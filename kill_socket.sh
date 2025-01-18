#!/bin/bash

# Define the port to clear
PORT=12346

echo "Attempting to clear port $PORT..."

# Check if the port is in use
PID=$(lsof -t -i:$PORT)

if [ -z "$PID" ]; then
  echo "Port $PORT is not in use. No action needed."
else
  echo "Port $PORT is in use by process ID(s): $PID"
  echo "Attempting to terminate process(es)..."

  # Try to terminate the process gracefully
  kill $PID 2>/dev/null
  sleep 1

  # Check if the process is still running
  if ps -p $PID > /dev/null 2>&1; then
    echo "Processes did not terminate gracefully. Forcing termination..."
    kill -9 $PID 2>/dev/null
  fi

  echo "Processes using port $PORT have been terminated."
fi

echo "Port $PORT is now clear."
