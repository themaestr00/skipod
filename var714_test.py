import pathlib
import argparse
import subprocess


LSF_SCRIPT = """#BSUB -n {M}
#BSUB -W 00:15
#BSUB -o "{name}.out"
#BSUB -e "{name}.err"
#BSUB -R "span[hosts=1]"
OMP_NUM_THREADS={N} ./{name}"""


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-N",
        "--num-threads",
        type=int,
        default=4,
        help="Number of threads to use for the test",
    )
    parser.add_argument(
        "--optimize",
        type=str,
        default="-O0",
        help="Optimization level for the compiler (e.g., -O0, -O1, -O2, -O3)",
    )
    args = parser.parse_args()
    if args.num_threads <= 0:
        raise ValueError("Number of threads must be a positive integer.")
    num_threads = args.num_threads
    tasks = ["var714", "var714_parallel", "var714_parallel_task"]
    subprocess.run(["make", "clean"], env={"PWD": str(pathlib.Path.cwd())})
    subprocess.run(["make"], env={"OPTIMIZE": args.optimize, "PWD": str(pathlib.Path.cwd())})
    for task in tasks:
        script_content = LSF_SCRIPT.format(M=num_threads // 8 + 1, N=num_threads, name=task)
        script_path = pathlib.Path(f"{task}.lsf")
        script_path.write_text(script_content)
        subprocess.run(["bsub", "<", str(script_path)], env={"PWD": str(pathlib.Path.cwd())})
        script_path.unlink()
    print(f"Submitted tasks with {num_threads} threads each.")
