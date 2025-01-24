#!/bin/bash

PAGE_TRACE_FILE="page_trace.txt"

if [ -f "$PAGE_TRACE_FILE" ]; then
    rm "$PAGE_TRACE_FILE"
fi

valgrind --tool=lackey --trace-mem=yes tree ~ 2>&1 | grep  "L" > "$PAGE_TRACE_FILE"