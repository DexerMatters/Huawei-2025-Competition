'''
D5C8-3085
'''

import math

def calculate_read_tokens(V, read_count):
    current_cell = 0
    previous_action = None
    previous_token = 0
    read_tokens = []

    for i in range(read_count):
        if previous_action != "Read":
            token = 64
        else:
            token = max(16, math.ceil(previous_token * 0.8))

        read_tokens.append(token)
        previous_token = token
        previous_action = "Read"
        current_cell = (current_cell % V) + 1

    return read_tokens

# Example usage
if __name__ == "__main__":
    V = 10  # Example value for V
    read_count = 20  # Number of reads to simulate
    tokens = calculate_read_tokens(V, read_count)
    print(tokens)