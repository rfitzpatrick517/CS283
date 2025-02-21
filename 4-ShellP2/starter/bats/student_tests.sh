#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Basic command execution" {
    run "./dsh" <<EOF
echo Hello
EOF

    [ "$status" -eq 0 ]
    [[ "$output" =~ "Hello" ]]
}

@test "Command with multiple spaces between arguments" {
    run "./dsh" <<EOF
echo    hello       world
EOF

    [ "$status" -eq 0 ]
    [[ "$output" =~ "hello world" ]]
}

@test "Command with leading and trailing spaces" {
    run "./dsh" <<EOF
   echo test   
EOF

    [ "$status" -eq 0 ]
    [[ "$output" =~ "test" ]]
}

@test "Command with quoted arguments" {
    run "./dsh" <<EOF
echo "hello     world"
EOF

    [ "$status" -eq 0 ]
    [[ "$output" =~ "hello     world" ]]
}

@test "cd with valid directory" {
    mkdir -p testdir
    run "./dsh" <<EOF
cd testdir
pwd
EOF

    [ "$status" -eq 0 ]
    [[ "$output" =~ "$(pwd)/testdir" ]]

    rmdir testdir
}

@test "cd with no arguments should stay in current working directory" {
    run "./dsh" <<EOF
cd
pwd
EOF

    [ "$status" -eq 0 ]
    [[ "$output" =~ "$HOME" ]]
}

@test "exit will exit successfully" {
    run "./dsh" <<EOF
exit
EOF

    [ "$status" -eq 0 ]
}

@test "Empty input shows warning" {
    run "./dsh" <<EOF

EOF

    [ "$status" -eq 0 ]
    [[ "$output" =~ "warning: no commands provided" ]]
}

@test "Multiple commands in one session" {
    run "./dsh" <<EOF
echo first
echo second
EOF

    [ "$status" -eq 0 ]
    [[ "$output" =~ "first" ]]
    [[ "$output" =~ "second" ]]
}

