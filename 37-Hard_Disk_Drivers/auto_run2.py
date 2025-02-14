import subprocess
import re
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import time
from datetime import timedelta

def run_command(w_value):
    cmd = [
        'python3', 'disk.py',
        '-A', '1000,-1,0',
        '-p', 'SATF',
        '-c',
        '-w', str(w_value)
    ]
    
    try:
        start_time = time.time()
        output = subprocess.check_output(cmd, universal_newlines=True, stderr=subprocess.STDOUT)
        exec_time = time.time() - start_time
        
        result = None
        for line in output.split('\n'):
            if line.startswith('TOTALS'):
                match = re.search(r'Total:\s*([\d.]+)', line)
                if match:
                    result = float(match.group(1))
        return result, exec_time
    except Exception as e:
        return None, f"Error: {str(e)}"

def main():
    print("=== Benchmark Started ===")
    start_total = time.time()
    results = []
    total_tests = 1001  # 0-1000共1001个测试
    last_status = ""
    
    try:
        for i, w in enumerate(range(0, 1001, 10)):
            # 进度计算
            progress = (i + 1) / total_tests * 100
            elapsed = time.time() - start_total
            avg_time = elapsed / (i + 1) if i > 0 else 0
            remaining = avg_time * (total_tests - i - 1)
            
            # 执行测试
            test_start = time.time()
            time_result, exec_info = run_command(w)
            test_duration = time.time() - test_start
            
            # 构建状态信息
            status_parts = [
                f"[{time.strftime('%H:%M:%S')}]",
                f"Progress: {progress:6.2f}%",
                f"Elapsed: {timedelta(seconds=int(elapsed))}",
                f"ETA: {timedelta(seconds=int(remaining))}",
                f"w={w:4d}",
            ]
            
            # 添加执行结果
            if isinstance(exec_info, str):
                status_parts.append(f"Last: {exec_info[:30]}")
            else:
                status_parts.append(f"Result: {time_result:.2f}ms" if time_result else "Result: None")
                status_parts.append(f"Exec: {exec_info:.2f}s")
            
            # 合并状态信息并截断长度
            status = " | ".join(status_parts).ljust(150)[:150]
            
            # 使用ANSI转义码清除行并移动光标到行首
            print(f"\r\033[K{status}", end="", flush=True)
            
            if time_result is not None:
                results.append({
                    'w_value': w,
                    'total_time': time_result,
                    'exec_duration': exec_info if isinstance(exec_info, float) else None
                })

    except KeyboardInterrupt:
        print("\n[WARNING] User interrupted the test!")
        print(f"Collected {len(results)} data points")
        
    finally:
        # 保存结果
        print("\n")  # 换行保证进度条不残留
        if results:
            df = pd.DataFrame(results)
            df.to_csv('w_benchmark.csv', index=False)
            print("[INFO] Results saved to w_benchmark.csv")
            
            # 绘图
            plt.figure(figsize=(12, 6))
            sns.lineplot(data=df, x='w_value', y='total_time', linewidth=1)
            plt.title(f'Disk Scheduler Performance ({len(results)} samples)')
            plt.xlabel('W Value')
            plt.ylabel('Total Time (ms)')
            plt.grid(True, alpha=0.3)
            plt.savefig('w_performance.png', bbox_inches='tight', dpi=300)
            print("[INFO] Visualization saved to w_performance.png")
            
        total_time = time.time() - start_total
        print(f"=== Benchmark Completed in {timedelta(seconds=int(total_time))} ===")

if __name__ == "__main__":
    main()