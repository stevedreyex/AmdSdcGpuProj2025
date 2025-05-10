import numpy as np
import matplotlib.pyplot as plt
import math
import pandas as pd
import seaborn as sns

# Performance data
data = {
    'num_elements': [2**x for x in range(10, 30)],
    'gpu_time': [
        0.0928, 0.108544, 0.126976, 0.147456, 0.169632, 0.192064, 0.211264, 
        0.244704, 0.342944, 0.511904, 0.839712, 1.51162, 2.92352, 5.9473, 
        12.7825, 62.8782, 133.967, 284.792, 606.112, 1287.37
    ],
    'cpu_time': [
        0.127, 0.276, 0.613, 1.236, 2.76, 5.674, 12.804, 28.19, 60.004, 
        130.747, 299.761, 595.151, 1265.44, 2693.2, 5742.7, 12278.5, 
        25993.6, 55022.3, 116949, 245618
    ]
}

# Create DataFrame
df = pd.DataFrame(data)

# Calculate performance metrics
def calculate_metrics(df):
    # Performance Ratio (CPU Time / GPU Time)
    df['performance_ratio'] = df['cpu_time'] / df['gpu_time']
    
    # Logarithmic scaling of performance
    df['log_num_elements'] = np.log2(df['num_elements'])
    df['log_gpu_time'] = np.log10(df['gpu_time'])
    df['log_cpu_time'] = np.log10(df['cpu_time'])
    
    # Linear regression to estimate time complexity
    gpu_slope, gpu_intercept = np.polyfit(df['log_num_elements'], df['log_gpu_time'], 1)
    cpu_slope, cpu_intercept = np.polyfit(df['log_num_elements'], df['log_cpu_time'], 1)
    
    return {
        'gpu_time_complexity': gpu_slope,
        'cpu_time_complexity': cpu_slope,
        'max_performance_ratio': df['performance_ratio'].max(),
        'mean_performance_ratio': df['performance_ratio'].mean(),
        'median_performance_ratio': df['performance_ratio'].median()
    }

# Visualize Performance
def plot_performance(df):
    plt.figure(figsize=(12, 6))
    
    # Time Comparison Plot
    plt.subplot(1, 2, 1)
    plt.plot(df['num_elements'], df['gpu_time'], label='GPU Time', color='blue')
    plt.plot(df['num_elements'], df['cpu_time'], label='CPU Time', color='red')
    plt.xscale('log')
    plt.yscale('log')
    plt.xlabel('Number of Elements')
    plt.ylabel('Time (ms)')
    plt.title('Bitonic Sort: Time Complexity')
    plt.legend()
    
    # Performance Ratio Plot
    plt.subplot(1, 2, 2)
    plt.plot(df['num_elements'], df['performance_ratio'], color='green')
    plt.xscale('log')
    plt.yscale('log')
    plt.xlabel('Number of Elements')
    plt.ylabel('Performance Ratio (CPU/GPU)')
    plt.title('Performance Speedup')
    
    plt.tight_layout()
    plt.show()

# Perform analysis
metrics = calculate_metrics(df)
plot_performance(df)

# Print detailed analysis
print("Bitonic Sort Performance Analysis:")
print("\nTime Complexity Estimation:")
print(f"GPU Time Complexity (log-log slope): {metrics['gpu_time_complexity']:.4f}")
print(f"CPU Time Complexity (log-log slope): {metrics['cpu_time_complexity']:.4f}")

print("\nPerformance Ratio (CPU Time / GPU Time):")
print(f"Maximum Performance Ratio: {metrics['max_performance_ratio']:.2f}")
print(f"Mean Performance Ratio: {metrics['mean_performance_ratio']:.2f}")
print(f"Median Performance Ratio: {metrics['median_performance_ratio']:.2f}")

# Detailed performance breakdown
print("\nDetailed Performance Breakdown:")
print(df[['num_elements', 'gpu_time', 'cpu_time', 'performance_ratio']].to_string(index=False))