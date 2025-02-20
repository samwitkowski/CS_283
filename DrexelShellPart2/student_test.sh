#!/usr/bin/env bats

# File: student_tests.sh
# Custom BATS test suite for dsh

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF
    [ "$status" -eq 0 ]
}

@test "Check pwd command" {
    run ./dsh <<EOF
pwd
EOF
    [ "$status" -eq 0 ]
}

@test "Change directory with cd" {
    mkdir -p testdir
    run ./dsh <<EOF
cd testdir
pwd
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "testdir" ]]
    rmdir testdir
}

@test "cd with invalid directory fails" {
    run ./dsh <<EOF
cd doesnotexist
EOF
    [ "$status" -ne 0 ]
    [[ "$output" =~ "cd failed" ]]
}

@test "Exit command should terminate shell" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Run echo with arguments" {
    run ./dsh <<EOF
echo "hello world"
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "hello world" ]]
}

@test "Run command with multiple arguments" {
    run ./dsh <<EOF
ls -l
EOF
    [ "$status" -eq 0 ]
}

@test "Handles quoted spaces correctly" {
    run ./dsh <<EOF
echo "hello    world"
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "hello    world" ]]
}
