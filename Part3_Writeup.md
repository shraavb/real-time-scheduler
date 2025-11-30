# Part 3: Discussion Questions

## Question 1: Task Deadline Misses

### 1(a) Task Set A with Termination Flag = True (0 to 20 time units)

**Task Set A:**
| Task | S | P | E | D |
|------|---|---|---|---|
| 1    | 0 | 2 | 1 | 2 |
| 2    | 0 | 6 | 2 | 6 |
| 3    | 0 | 8 | 3 | 8 |

**Results:**
- **RM (Rate Monotonic):** 2 deadline misses
- **EDF (Earliest Deadline First):** 3 deadline misses

### 1(b) Task Set A with Termination Flag = False

**Results:**
- **RM (Rate Monotonic):** 2 deadline misses
- **EDF (Earliest Deadline First):** 6 deadline misses

**Explanation:**

For **RM**, the number of deadline misses remains the same (2) regardless of the termination flag. This is because RM uses static priorities based on period. The same tasks always get preempted in the same way, so terminating a job that missed its deadline doesn't change which future jobs will miss their deadlines. The pattern of deadline misses repeats deterministically.

For **EDF**, the number of deadline misses **increases** from 3 to 6 when the termination flag is set to false. This happens because EDF uses dynamic priorities based on absolute deadlines. When a job misses its deadline but isn't terminated:
1. It continues to execute and completes its remaining execution time
2. This delays other jobs that would otherwise have met their deadlines
3. The delayed jobs then miss their own deadlines, creating a cascading effect
4. This results in additional deadline misses that wouldn't have occurred if the late job had been terminated

### 1(c) Reason to Choose RM over EDF

Based on the results from part (1b), one key reason a system designer would choose **RM over EDF** is **predictability and containment of deadline misses**.

With RM, deadline misses are more contained and don't cascade. When a system becomes overloaded or a deadline is missed, the static priority structure of RM ensures that the same pattern repeats. This makes the system behavior more predictable and easier to reason about, even under overload conditions.

In contrast, EDF's dynamic priorities can lead to cascading deadline misses when tasks are not terminated upon missing deadlines. A single missed deadline can trigger a chain reaction of additional misses, making the system behavior less predictable under stress.

### 1(d) Task Termination Trade-offs

**Why terminate tasks after missing deadlines:**
1. **Prevent cascading failures:** As seen in question 1(b), not terminating late tasks can cause additional deadline misses
2. **Resource recovery:** Terminating a late task frees up CPU resources for upcoming jobs that can still meet their deadlines
3. **Bounded impact:** Limits the damage from one missed deadline to that specific job instance
4. **Fresh start:** The next period's job starts with full execution time available rather than competing with the late job

**Why continue executing tasks after missing deadlines:**
1. **Value of completion:** In some systems, completing the task late is better than not completing it at all (e.g., processing sensor data, rendering frames)
2. **Eventual consistency:** The task may eventually catch up if the overload is temporary
3. **No wasted work:** The computation already performed isn't thrown away
4. **Soft real-time requirements:** Some systems have soft deadlines where late completion is acceptable, just not preferred

## Question 2: Theoretical Schedulability Analysis

### 2(a) CPU Utilization Calculations

**Task Set B:**
| Task | S | P | E | D | Ei/Pi |
|------|---|---|---|---|-------|
| 1    | 0 | 4 | 1 | 4 | 1/4 = 0.250 |
| 2    | 0 | 6 | 2 | 6 | 2/6 ≈ 0.333 |
| 3    | 0 | 8 | 3 | 8 | 3/8 = 0.375 |

**Total CPU Utilization (B):** U = 0.250 + 0.333 + 0.375 = **0.958** (95.8%)

**Task Set C:**
| Task | S | P | E | D | Ei/Pi |
|------|---|---|---|---|-------|
| 1    | 0 | 4 | 1 | 4 | 1/4 = 0.250 |
| 2    | 0 | 6 | 2 | 6 | 2/6 ≈ 0.333 |
| 3    | 0 | 8 | 4 | 8 | 4/8 = 0.500 |

**Total CPU Utilization (C):** U = 0.250 + 0.333 + 0.500 = **1.083** (108.3%)

### 2(b) EDF Schedulability Analysis

**Task Set B with EDF:**
- CPU Utilization: 0.958 (< 1.0)
- **Expected deadline misses:** **NO**
- **Reasoning:** EDF is optimal for periodic tasks with deadlines equal to periods. The theoretical schedulability bound for EDF is U ≤ 1.0. Since task set B has U = 0.958 < 1.0, all deadlines can be met with EDF scheduling.

**Task Set C with EDF:**
- CPU Utilization: 1.083 (> 1.0)
- **Expected deadline misses:** **YES**
- **Reasoning:** Task set C has U = 1.083 > 1.0, which exceeds the theoretical bound for EDF. This means the system requires more than 100% of the CPU capacity, making it impossible to meet all deadlines regardless of the scheduling algorithm used.

### 2(c) RM Schedulability Analysis

**Theoretical RM Bound:**

For n tasks, the RM schedulability bound is: U ≤ n(2^(1/n) - 1)

For n = 3 tasks: U ≤ 3(2^(1/3) - 1) ≈ 3(1.260 - 1) = 3(0.260) ≈ **0.780** (78.0%)

**Task Set B with RM:**
- CPU Utilization: 0.958
- RM Bound: 0.780
- **Can we determine schedulability from utilization alone?** **NO**
- **Reasoning:** The CPU utilization (0.958) exceeds the RM sufficient bound (0.780). However, this bound is only a *sufficient* condition, not a *necessary* condition. Some task sets with U > 0.780 can still be schedulable with RM, while others cannot. We would need to perform a more detailed schedulability test (such as response time analysis or simulation) to determine if task set B is actually schedulable under RM.

**Task Set C with RM:**
- CPU Utilization: 1.083
- **Can we determine schedulability from utilization alone?** **YES**
- **Schedulability:** **NOT SCHEDULABLE**
- **Reasoning:** Task set C has U = 1.083 > 1.0, which exceeds 100% CPU utilization. No scheduling algorithm (including RM) can schedule a task set that requires more CPU time than available. This is a *necessary* condition for schedulability, so we can definitively say task set C is not schedulable under RM (or any other algorithm).

---

**Summary:**
- EDF can use utilization alone for schedulability when U ≤ 1.0
- RM's sufficient bound (≈78% for 3 tasks) is conservative; exceeding it doesn't guarantee failure
- Any utilization > 100% guarantees the task set is not schedulable regardless of algorithm
