from random import choice, randint


edges_number = randint(100, 500)
start_test_number = int(input("Enter start test number:\n"))
tests_number = int(input("Enter tests number:\n"))

prev = 0
edges = []

for i in range(start_test_number, start_test_number + tests_number):
    with open(f"./in/{i}test.in", "w") as test_file:
        for start in range(1, edges_number + 1):
            outgoing_num = randint(1, 20)
            for i in range(1):
                end = randint(1, 500)
                while [start, end] in edges:
                    end = randint(start + 1, 1000)
                test_file.write(f"{start} -- {end}, 0\n")
                edges.append([start, end])
        test_file.write("\n")
