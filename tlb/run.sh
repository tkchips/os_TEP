#!/bin/bash

# 启用 Verbose 模式（注释掉下一行可关闭）
VERBOSE=1

# 输出帮助信息
if [[ $# -ne 0 ]]; then
    echo "Usage: $0"
    echo "Description: Run TLB test with pages from 1 to 1024"
    exit 1
fi

# 初始化结果文件
RESULTS_FILE="results.txt"
echo -n "" > "$RESULTS_FILE"

# 定义测试参数
PAGE_LIST=(1 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536)
NUM_RUNS=2  # 每个页数的测试次数

# Verbose 输出函数
log() {
    if [[ $VERBOSE -eq 1 ]]; then
        echo "[$(date +'%T')] $1" >&2  # 输出到 stderr 避免干扰结果
    fi
}

log "Starting TLB measurement test..."
log "Testing pages: ${PAGE_LIST[*]}"
log "Each page count will run $NUM_RUNS times"

# 主测试循环
for pages in "${PAGE_LIST[@]}"; do
    log "-----------------------------------------------------------------"
    log "Testing $pages pages..."
    
    for ((run=1; run<=NUM_RUNS; run++)); do
        log "  Run $run/$NUM_RUNS with $pages pages..."
        
        # 运行测试程序并捕获输出
        output=$(./tlb "$pages" 100000 2>&1)
        
        # 检查程序是否成功
        if [[ $? -ne 0 ]]; then
            log "  ERROR: Test failed for $pages pages (run $run)"
            echo "$pages -1" >> "$RESULTS_FILE"  # 写入错误标记
            continue
        fi
        
        # 输出原始结果（同时显示在 verbose 模式）
        log "  Raw output: $output"
        echo "$output" >> "$RESULTS_FILE"
    done
done

log "-----------------------------------------------------------------"
log "Processing results with awk..."
awk '{pages=$1; time=$2; sum[pages]+=time; count[pages]++} 
     END {for (p in sum) print p, sum[p]/count[p]}' "$RESULTS_FILE" \
     | sort -n > final_results.txt

log "Test completed. Final results saved to final_results.txt"
log "Use 'python plot_tlb.py' to generate the plot"