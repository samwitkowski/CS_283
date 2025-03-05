#!/usr/bin/env bats

@test "Verify ls | grep executes correctly" {
    run ./dsh <<EOF
ls | grep ".c"
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ ".c" ]]
}

@test "Ensure extra spaces in pipeline do not affect execution" {
    run ./dsh <<EOF
ls    |    grep    ".c"
EOF
    [ "$status" -eq 0 ]
}

@test "Confirm cd changes the directory" {
    mkdir -p testdir
    run ./dsh <<EOF
cd testdir
pwd
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "testdir" ]]
    rmdir testdir
}

@test "Handle invalid directory for cd" {
    run ./dsh <<EOF
cd doesnotexist
EOF
    [ "$status" -ne 0 ]
    [[ "$output" =~ "cd failed" ]]
}

@test "Validate exit command terminates the shell" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Verify multi-stage pipeline execution" {
    run ./dsh <<EOF
ls | grep ".c" | wc -l
EOF
    [ "$status" -eq 0 ]
    output_trimmed=$(echo "$output" | tr -d '[:space:]')
    [[ "$output_trimmed" =~ ^[0-9]+$ ]] || false
}
