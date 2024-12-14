import os
import subprocess

# Path to the output header file
header_file = "firmware/include/git_info.h"

def get_git_info():
    try:
        # Get the current Git branch name
        branch_name = subprocess.check_output(["git", "rev-parse", "--abbrev-ref", "HEAD"], stderr=subprocess.STDOUT)
        branch_name = branch_name.strip().decode("utf-8")
    except (subprocess.CalledProcessError, FileNotFoundError) as e:
        branch_name = "unknown"
        print(e)


    try:
        # Get the short commit ID
        commit_id = subprocess.check_output(["git", "rev-parse", "--short", "HEAD"], stderr=subprocess.STDOUT)
        commit_id = commit_id.strip().decode("utf-8")
    except (subprocess.CalledProcessError, FileNotFoundError) as e:
        commit_id = "unknown"
        print(e)

    return branch_name, commit_id

# Get Git branch and commit ID
branch_name, commit_id = get_git_info()

# Ensure the include directory exists
os.makedirs(os.path.dirname(header_file), exist_ok=True)

# Write the branch name and commit ID to the header file
with open(header_file, "w") as f:
    f.write(f'#define GIT_BRANCH "{branch_name}"\n')
    f.write(f'#define GIT_COMMIT_ID "{commit_id}"\n')

print(f"Generated {header_file} with branch name: {branch_name} and commit ID: {commit_id}")