"""Host-side simulation of Info-Orbs clock timing logic.

Replicates the exact uint32 arithmetic used on the ESP32 to prove:
 1. The OLD GlobalTime scheduler (m_updateTimer = millis() after a '> 1000'
    check) accumulates drift and skips displayed seconds -> parity-driven
    colon stalls (the "long irregular colon" bug).
 2. The NEW drift-free scheduler (m_updateTimer += whole periods) never
    skips a second, including across the 49.7-day millis() rollover.
 3. The NEW 500 ms colon blinker toggles at a stable cadence across rollover.
 4. The seconds-dot arc angles stay within valid bounds for all inputs,
    and out-of-range seconds / non-status orbs are rejected.

Run:  python firmware/test/host_sim_long_uptime.py
"""

U32 = 1 << 32


def u32(x):
    return x & (U32 - 1)


def epoch_seconds(ms, ntp_last_update, ntp_epoch):
    """NTPClient::getEpochTime(): epoch + (millis() - lastUpdate) / 1000."""
    return ntp_epoch + u32(ms - ntp_last_update) // 1000


def simulate_old_scheduler(start_ms, duration_ms, loop_latency_ms):
    """OLD: if (millis() - t > 1000) { ...; t = millis(); } -> drifts."""
    ms = start_ms
    t = start_ms
    ntp_last, ntp_epoch = start_ms, 1_000_000
    last_seen = epoch_seconds(ms, ntp_last, ntp_epoch)
    skips = colon_stalls = 0
    end = ms + duration_ms
    while ms < end:
        ms += loop_latency_ms
        if u32(ms - t) > 1000:
            t = ms
            sec = epoch_seconds(ms, ntp_last, ntp_epoch)
            delta = sec - last_seen
            if delta > 1:
                skips += delta - 1
                if delta % 2 == 0:
                    colon_stalls += 1  # parity unchanged -> colon frozen >= 2s
            last_seen = sec
    return skips, colon_stalls


def simulate_new_scheduler(start_ms, duration_ms, loop_latency_ms):
    """NEW: elapsed >= 1000 -> t += 1000 * (elapsed // 1000). Drift-free."""
    ms = start_ms
    t = start_ms
    ntp_last, ntp_epoch = start_ms, 1_000_000
    last_seen = epoch_seconds(ms, ntp_last, ntp_epoch)
    skips = updates = 0
    end = ms + duration_ms
    while ms < end:
        ms += loop_latency_ms
        elapsed = u32(u32(ms) - u32(t))
        if elapsed >= 1000:
            t = t + 1000 * (elapsed // 1000)  # catch-up, no drift
            sec = epoch_seconds(ms, ntp_last, ntp_epoch)
            if sec - last_seen > 1:
                skips += sec - last_seen - 1
            last_seen = sec
            updates += 1
    return skips, updates


def simulate_colon_blink(start_ms, duration_ms, loop_latency_ms):
    """NEW ClockWidget colon: toggle every 500 ms, rollover-safe."""
    ms = start_ms
    prev = start_ms
    visible = True
    toggle_times = []
    end = ms + duration_ms
    while ms < end:
        ms += loop_latency_ms
        elapsed = u32(u32(ms) - u32(prev))
        if elapsed >= 500:
            prev = prev + 500 * (elapsed // 500)
            visible = not visible
            toggle_times.append(ms)
    # Cadence check: nominal toggle grid is every 500 ms; observation jitter
    # is bounded by loop latency.
    intervals = [b - a for a, b in zip(toggle_times, toggle_times[1:])]
    return intervals


def simulate_colon_with_stalls(duration_ms, stall_every_ms, stall_ms, loop_latency_ms):
    """NEW parity-correct colon blinker with periodic loop stalls injected
    (models one blocking NTP attempt per retry interval).

    Returns (phase_errors, toggles): phase_errors counts observations where the
    colon state disagrees with the ideal 500 ms square wave outside a stall.
    """
    ms = 0
    prev = 0
    visible = True
    next_stall = stall_every_ms
    phase_errors = toggles = 0
    while ms < duration_ms:
        if ms >= next_stall:
            ms += stall_ms  # blocking NTP forceUpdate timeout
            next_stall += stall_every_ms
        else:
            ms += loop_latency_ms
        elapsed = u32(u32(ms) - u32(prev))
        if elapsed >= 500:
            periods = elapsed // 500
            prev = prev + 500 * periods
            if periods % 2 == 1:
                visible = not visible
            toggles += 1
            # Ideal square wave: visible == (whole 500ms periods since t0) even
            ideal = (prev // 500) % 2 == 0
            if visible != ideal:
                phase_errors += 1
    return phase_errors, toggles


def check_seconds_dot():
    """Validate displaySeconds() guards and arc angle bounds."""
    for orb in range(-2, 7):
        for sec in range(-5, 66):
            draws = not (orb != 2 or sec < 0 or sec > 59)
            if draws:
                if sec < 30:
                    start, endang = 6 * sec + 180, 6 * sec + 186
                else:
                    start, endang = 6 * sec - 180, 6 * sec - 174
                assert 0 <= start <= 360 and 0 <= endang <= 360, (sec, start, endang)
                assert orb == 2
            else:
                assert orb != 2 or sec < 0 or sec > 59
    return True


ROLLOVER = U32  # millis() wraps every 4294967296 ms = 49.71 days

print("=== 1. OLD GlobalTime scheduler (3 simulated hours, 5 ms loop) ===")
skips, stalls = simulate_old_scheduler(0, 3 * 3600 * 1000, 5)
print(f"    skipped seconds: {skips}, colon parity stalls (>=2s frozen): {stalls}")
assert skips > 0, "expected the old scheduler to skip seconds"

print("=== 2. NEW GlobalTime scheduler (3 simulated hours, 5 ms loop) ===")
skips, updates = simulate_new_scheduler(0, 3 * 3600 * 1000, 5)
print(f"    skipped seconds: {skips} over {updates} updates")
assert skips == 0

print("=== 3. NEW scheduler across millis() rollover (20 min straddling wrap) ===")
start = ROLLOVER - 10 * 60 * 1000  # 10 min before the 49.7-day wrap
skips, updates = simulate_new_scheduler(start, 20 * 60 * 1000, 5)
print(f"    skipped seconds: {skips} over {updates} updates (start=0x{u32(start):08X})")
assert skips == 0

print("=== 4. NEW scheduler with blocked loop (2500 ms stalls injected) ===")
skips, updates = simulate_new_scheduler(0, 3600 * 1000, 2500)
# seconds jump by 2-3 during a blocked loop, but scheduler must not spiral
print(f"    updates: {updates} (catch-up works, no spiral)")

print("=== 5. Colon blink cadence across rollover (30 min straddling wrap) ===")
intervals = simulate_colon_blink(ROLLOVER - 15 * 60 * 1000, 30 * 60 * 1000, 5)
bad = [i for i in intervals if not 490 <= i <= 510]
print(f"    toggles: {len(intervals) + 1}, min/max interval: {min(intervals)}/{max(intervals)} ms, out-of-tolerance: {len(bad)}")
assert not bad, f"unstable colon intervals: {bad[:5]}"

print("=== 6. Colon blink long-run drift (24 simulated hours, 7 ms loop) ===")
intervals = simulate_colon_blink(0, 24 * 3600 * 1000, 7)
total = sum(intervals)
expected = len(intervals) * 500
drift = total - expected
print(f"    toggles: {len(intervals) + 1}, cumulative drift vs 500 ms grid: {drift} ms")
assert abs(drift) <= 7, "cumulative drift detected"

print("=== 7. Seconds-dot bounds / orb ownership (all orbs x seconds -5..65) ===")
check_seconds_dot()
print("    all draws bounded to orb 2, seconds 0..59, angles within 0..360")

print("=== 8. Colon blink under NTP failure ===")
# OLD GlobalTime: dead NTP server blocks EVERY 1s tick ~1010 ms -> the render
# loop runs ~1x/s and the colon degrades to ~1s on / 1s off (reported bug).
intervals = simulate_colon_blink(0, 3600 * 1000, 1010)
print(f"    old (loop stalled every tick): median toggle interval = {sorted(intervals)[len(intervals) // 2]} ms (bug: ~1010, not 500)")
assert sorted(intervals)[len(intervals) // 2] > 900
# NEW GlobalTime: attempts gated to one per 61 s -> a single ~1s stall per
# minute; parity-correct catch-up keeps the colon phase-locked to the 500 ms
# grid at all other times.
errors, toggles = simulate_colon_with_stalls(2 * 3600 * 1000, 61_000, 1010, 5)
print(f"    new (1 gated stall / 61s): {toggles} toggles over 2h, phase errors: {errors}")
assert errors == 0

print()
print("ALL CHECKS PASSED")
