#!/bin/bash

start() {
    echo "Starting saphy..."
    
    saphy
}

wait() {
    echo "wait saphy..."
}

stop() {
    echo "Stopping saphy..."
    kill -9 `ps -ef | grep /usr/local/bin/saphy | awk '{print $2}'`
    echo "Stopped saphy..."
    exit 0
}

case "$1" in
    start|wait|stop)
        $1
        ;;
    *)
        echo "Usage: $0 {start|wait|stop}"
        exit 1
        ;;
esac
