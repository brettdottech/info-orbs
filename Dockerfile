FROM python:3.11-slim

# Set working directory
WORKDIR /app

# Copy the proxy server script
COPY nhl-proxy-server.py .

# Install dependencies
RUN pip install --no-cache-dir flask requests

# Expose port 5000
EXPOSE 5000

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
  CMD python -c "import requests; requests.get('http://localhost:5000/health', timeout=5)"

# Run the server
CMD ["python", "nhl-proxy-server.py"]
