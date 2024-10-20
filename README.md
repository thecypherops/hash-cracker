# Hash Tool Performance Analysis Report

This report summarizes the performance of the hash tool, with insights drawn from various profiling and monitoring tools.

## 1. CoreFreq
CoreFreq provides detailed CPU monitoring capabilities.

### Command Used
```bash
sudo ../build/corefreqd  # starts the daemon
./build/corefreq-cli     # starts the client
```

### Results
![\[CPU frequency and performance metrics\]](corefreq.png)

## 2. mpstat
The mpstat command reports processor statistics.

### Command Used
```bash
mpstat -P ALL 1
```

### Results
![\[per-processor stats\]](mpstat.png)

## 3. iostat
Iostat monitors system input/output device loading.

### Command Used
```bash
iostat 1
```

### Results
![\[I/O statists\]](performance_output/iostat.png)

## 4. htop
Htop provides an interactive process viewer and system monitor.

### Results
![\[system resource usage\]](performance_output/htop.png)

## 5. gprof
Gprof performs CPU profiling analysis.

### Command Used
```bash
gprof ./hash_tool gmon.out > hash_tool_gprof.txt
```

### Results
![\[performance data\]](performance_output/gprof.png)

## 6. valgrind
Valgrind performs memory analysis and leak detection.

### Command Used
```bash
valgrind --leak-check=yes ./hash_tool
```

### Results
![\[memory analysis\]](performance_output/valgrind.png)

## 7. SAR (System Activity Reporter)
SAR collects and reports system activity information.

### Command Used
```bash
sar -A -u ALL 1 > hash_tool_sar.txt
```

### Additional Analysis
- Graph created using Sargraph tool
![\[SAR graph visualization\]](performance_output/percentage-of-cpu-all-ut.jpeg)

## 8. perf stat
Perf stat provides basic performance statistics.

### Command Used
```bash
perf stat ./hash_tool
```

### Results
![\[perf stat output\]](performance_output/perf.png)

## 9. time
Time command measures program execution time.

### Command Used
```bash
time -p ./hash_tool
```

### Results
![\[time output\]](performance_output/time.png)

## 10. perf record with Hotspot GUI
Perf record with Hotspot provides detailed CPU profiling visualization.

### Command Used
```bash
perf record --call-graph dwarf ./hash_tool
```

### Analysis
- CPU utilization per thread can be observed using Hotspot tool
![\[Hotspot visualization\]](performance_output/hotspot_perf.png)


### Compilation Command
```bash
g++ -o hash_tool hash_tool.cpp -lcrypto -pthread
```
### Execution Command
```bash
./hash_tool -w /path/to/dictionary/list_of_passwords.txt -h 111338d75b3bb4949966c35f20494c153cd1c9b58966c74c917fa5c72d71ffe6 -t 8
```

---
