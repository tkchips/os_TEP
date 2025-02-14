import argparse
import subprocess
import re
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from itertools import product

def parse_arguments():
    parser = argparse.ArgumentParser(description='Run disk.py benchmarks.')
    parser.add_argument('--a', required=True, help='分号分隔的-a参数列表，例如 "0;1,2;3,4"')
    parser.add_argument('--S', help='分号分隔的-S参数列表，例如 "2;4;8;10"')
    parser.add_argument('--R', help='分号分隔的-R参数列表，例如 "0.1;0.2"')
    return parser.parse_args()

def run_command(params):
    cmd = ['python3', 'disk.py', '-c', '-a', params['a']]
    if params['S'] != 'None':
        cmd.extend(['-S', params['S']])
    if params['R'] != 'None':
        cmd.extend(['-R', params['R']])
    
    try:
        output = subprocess.check_output(cmd, universal_newlines=True, stderr=subprocess.STDOUT)
        for line in output.split('\n'):
            if line.startswith('TOTALS'):
                match = re.search(r'Total:\s*([\d.]+)', line)
                if match:
                    return float(match.group(1))
    except Exception as e:
        print(f"Error running {' '.join(cmd)}: {str(e)}")
    return None

def main():
    args = parse_arguments()
    
    # 生成所有参数组合
    a_list = args.a.split(';')
    S_list = ['None'] + args.S.split(';') if args.S else ['None']
    R_list = ['None'] + args.R.split(';') if args.R else ['None']
    
    # 生成参数矩阵
    param_matrix = list(product(a_list, S_list, R_list))
    
    results = []
    for a, S, R in param_matrix:
        params = {'a': a, 'S': S, 'R': R}
        time = run_command(params)
        if time is not None:
            results.append({
                'a-Parameter': a,
                'S-Parameter': S,
                'R-Parameter': R,
                'Total-Time': time
            })
    
    # 保存结果到CSV
    df = pd.DataFrame(results)
    
    # 生成图表
    # 生成图表
    sns.set(style="whitegrid")
    fig = plt.figure(figsize=(10, 6))  # 调整画布尺寸
    
    # 动态创建子图数量
    subplot_count = 0
    if args.S: subplot_count += 1
    if args.R: subplot_count += 1
    current_plot = 1
    
    # S参数分析
    if args.S:
        plt.subplot(subplot_count, 1, current_plot)
        current_plot += 1
        s_data = df[df['S-Parameter'] != 'None']
        sns.barplot(x='S-Parameter', y='Total-Time', hue='a-Parameter', data=s_data)
        plt.title('S Parameter Comparison', pad=10)
        plt.xlabel('')
        plt.ylabel('Time (ms)')
    
    # R参数分析
    if args.R:
        plt.subplot(subplot_count, 1, current_plot)
        r_data = df[df['R-Parameter'] != 'None']
        sns.barplot(x='R-Parameter', y='Total-Time', hue='a-Parameter', data=r_data)
        plt.title('R Parameter Comparison', pad=10)
        plt.ylabel('Time (ms)')
    
    # 调整布局并保存
    plt.tight_layout(pad=1.5)
    plt.savefig('benchmark_comparison.png', 
                bbox_inches='tight', 
                pad_inches=0.1,  # 进一步减少填充
                dpi=300)
    plt.close()

if __name__ == "__main__":
    main()