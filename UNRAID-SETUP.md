# Running NHL Proxy on Unraid

Perfect! Unraid is an excellent choice for running this proxy server. Here's how to set it up.

## Method 1: Using Docker Compose (Recommended)

### Step 1: Enable Docker Compose on Unraid

1. Install the **Compose Manager** plugin from Community Applications
2. Or use the built-in Docker Compose support (Unraid 6.11+)

### Step 2: Create the Container

1. **Copy files to Unraid:**
   ```bash
   # SSH into your Unraid server
   ssh root@your-unraid-ip
   
   # Create directory
   mkdir -p /mnt/user/appdata/nhl-proxy
   cd /mnt/user/appdata/nhl-proxy
   
   # Copy the files (use WinSCP, FileZilla, or the Unraid web interface)
   # You need: nhl-proxy-server.py, Dockerfile, docker-compose.yml
   ```

2. **Build and start the container:**
   ```bash
   cd /mnt/user/appdata/nhl-proxy
   docker-compose up -d
   ```

3. **Check if it's running:**
   ```bash
   docker ps | grep nhl-proxy
   docker logs nhl-proxy
   ```

4. **Test the endpoint:**
   ```bash
   curl http://localhost:5000/nhl/team/UTA
   ```

## Method 2: Using Unraid Docker Template (Manual Setup)

### Step 1: Add Container via Unraid Web UI

1. Go to **Docker** tab in Unraid
2. Click **Add Container**
3. Fill in the following:

**Container Settings:**
- **Name:** `nhl-proxy`
- **Repository:** `python:3.11-slim`
- **Network Type:** `Bridge`
- **Console shell command:** `Bash`

**Port Mappings:**
- **Container Port:** `5000`
- **Host Port:** `5000`
- **Connection Type:** `TCP`

**Path Mappings:**
- **Container Path:** `/app`
- **Host Path:** `/mnt/user/appdata/nhl-proxy`
- **Access Mode:** `Read/Write`

**Post Arguments:**
```bash
bash -c "pip install flask requests && python /app/nhl-proxy-server.py"
```

### Step 2: Upload Files

1. Navigate to `/mnt/user/appdata/nhl-proxy/` using the Unraid web interface or SSH
2. Upload `nhl-proxy-server.py` to this directory
3. Start the container

## Method 3: Using Pre-built Docker Image (Easiest)

If you want to skip building, you can use the Dockerfile to create an image:

```bash
# SSH into Unraid
cd /mnt/user/appdata/nhl-proxy

# Build the image
docker build -t nhl-proxy:latest .

# Run the container
docker run -d \
  --name nhl-proxy \
  --restart unless-stopped \
  -p 5000:5000 \
  -e TZ=America/Denver \
  nhl-proxy:latest
```

## Accessing the Proxy

### From Your Local Network:
```
http://UNRAID-IP:5000/nhl/team/UTA
```

### From Outside Your Network (Work):

You have two options:

#### Option A: Port Forwarding
1. Forward port 5000 on your router to your Unraid server
2. Use your home's public IP: `http://YOUR-HOME-IP:5000/nhl/team/UTA`
3. (Recommended) Set up Dynamic DNS for a friendly URL

#### Option B: Reverse Proxy (More Secure)
If you already have a reverse proxy (nginx, Caddy, Traefik):

**Nginx Proxy Manager Example:**
1. Add a new proxy host
2. Domain: `nhl.yourdomain.com`
3. Forward to: `unraid-ip:5000`
4. Enable SSL with Let's Encrypt
5. Access via: `https://nhl.yourdomain.com/nhl/team/UTA`

## Monitoring and Maintenance

### View Logs:
```bash
docker logs -f nhl-proxy
```

### Restart Container:
```bash
docker restart nhl-proxy
```

### Update Container:
```bash
cd /mnt/user/appdata/nhl-proxy
docker-compose pull
docker-compose up -d
```

### Check Health:
```bash
curl http://UNRAID-IP:5000/health
```

## Unraid Dashboard Integration

Add to your Unraid dashboard:

1. Install **Homepage** or **Heimdall** from Community Applications
2. Add a new service:
   - **Name:** NHL Proxy
   - **URL:** `http://UNRAID-IP:5000`
   - **Icon:** Use NHL logo
   - **Description:** NHL API Proxy for InfoOrbs

## Troubleshooting

### Container won't start:
```bash
# Check logs
docker logs nhl-proxy

# Check if port is in use
netstat -tulpn | grep 5000

# Rebuild container
docker-compose down
docker-compose up -d --build
```

### Can't access from outside:
1. Verify port forwarding is set up correctly
2. Check Unraid firewall settings
3. Test from inside network first
4. Check router firewall rules

### High memory usage:
The container should use ~50-100MB of RAM. If higher:
```bash
docker stats nhl-proxy
```

## Security Recommendations

1. **Use a reverse proxy** with SSL (Nginx Proxy Manager, Caddy)
2. **Add authentication** (I can modify the script to add API key auth)
3. **Limit access** to specific IP ranges if possible
4. **Monitor logs** for unusual activity
5. **Keep container updated** regularly

## Next Steps

Once the proxy is running on Unraid:

1. **Find your Unraid server's IP** (e.g., `192.168.1.50`)
2. **Test the endpoint:** `http://192.168.1.50:5000/nhl/team/UTA`
3. **Set up port forwarding** if accessing from work
4. **Update ESP32 firmware** to use the proxy URL
5. **Re-enable NHL widget** in the config

Your InfoOrbs will now be able to access NHL data through your Unraid server!
