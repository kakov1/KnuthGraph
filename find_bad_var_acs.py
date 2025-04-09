from random import randint
import subprocess

while True:
    with open(f"./build/src/test", "w") as test_file:
        edges_number = randint(1, 10)
        for start in range(1, edges_number + 1):
            outgoing_num = randint(1, 10)
            end = randint(1, 10)
            test_file.write(f"{outgoing_num} -- {end}, 0\n")
        test_file.write("\n")
        
    with open(f"./build/src/test", "r") as input_file:
        result = subprocess.run(
            ["./build/src/main"], stdin=input_file, capture_output=True, text=True
        )
        print("STDOUT:\n", result.stdout)
        if result.stderr:
            print("STDERR:\n", result.stderr)
            exit()
