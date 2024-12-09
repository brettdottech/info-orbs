<?php
$sensorsStr = shell_exec('/usr/bin/sensors -j');
$sensors = json_decode($sensorsStr, true);

$uptimeOutput = shell_exec('uptime');
if (preg_match('/^\s*(..:..:..) up (.*),  ([0-9]+ users),  load average: ([0-9.]+), ([0-9.]+), ([0-9.]+)$/', $uptimeOutput, $displays)) {
	$time = $displays[1];
	$uptime = preg_replace('/, /', " ", $displays[2]);
	$load1min = $displays[4];
	$load5min = $displays[5];
	$load15min = $displays[6];
}

$uptime = "line 1\nline 2 longer line\nline 3\n";
$uptime = [
	[
		"type" => "circle",
		"x" => 50,
		"y" => 50,
		"radius" => 20,
		"color" => "red",
		"filled" => true,
	],
	[
		"type" => "text",
		"x" => 120,
		"y" => 170,
		"font" => 1,
		"text" => "TestMe ",
		"color" => "white",
		"background" => "darkgrey",
	],
	[
		"type" => "rectangle",
		"x" => 70,
		"y" => 180,
		"height" => 50,
		"width" => 100,
		"filled" => true,
		"color" => "blue"
	],
	[
		"type" => "triangle",
		"x" => 120,
		"y" => 120,
		"x2" => 180,
		"y2" => 60,
		"x3" => 60,
		"y3" => 60,
		"filled" => false,
		"color" => "red"
	],
	[
		"type" => "line",
		"x" => 60,
		"y" => 120,
		"x2" => 180,
		"y2" => 120,
		"color" => "blue"
	],
	[
		"type" => "arc",
		"x" => 120,
		"y" => 120,
		"radius" => 30,
		"innerRadius" => 20,
		"angleStart" => 270,
		"angleEnd" => 90,
		"color" => "blue"
	],
	[
		"type" => "character",
		"x" => 120,
		"y" => 190,
		"character" => "E",
		"color" => "green",
		"font" => 2,
	],
];


$displays = [
	[
		'label' => 'CPU',
		'data' => $sensors['coretemp-isa-0000']['Package id 0']['temp1_input'],
		'labelColor' => 'blue',
		'color' => 'red',
		'background' => 'silver',
	],
	[
		'label' => 'PNY NVME',
		'data' => $sensors['nvme-pci-0200']['Composite']['temp1_input'],
	],
	[
		'label' => 'WD Blue NVME',
		'data' => $sensors['nvme-pci-0300']['Composite']['temp1_input'],
	],
	[
		'label' => 'Uptime',
		'data' => $uptime,
		'labelX' => 100,
		'labelY' => 100,
	],
	[
		'label' => 'Load 1 min',
		'data' => $load1min,
		'color' => 'green',
		'labelColor' => 'dArK Green',
		'fullDraw' => true,
	],
];

header('Content-Type: application/json; charset=utf-8');
print json_encode(['interval' => 5000, 'displays' => $displays]) . "\n";
