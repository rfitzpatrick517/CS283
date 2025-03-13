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

# TESTS FROM SHELLP3
# Check that `pwd` runs without errors
@test "check pwd runs without errors" {
    run ./dsh <<EOF
pwd
EOF

    # Assertions
    [ "$status" -eq 0 ] # Exit status should be 0
}

# Check that `echo` works correctly
@test "check echo outputs correctly" {
    run ./dsh <<EOF
echo Hello World
EOF

    # Assertions
    [ "$status" -eq 0 ] # Exit status should be 0
    [[ "${lines[0]}" == "Hello World" ]] # Output should match
}

# Check pipeline functionality with multiple pipes
@test "check ls | grep | wc works correctly" {
    run ./dsh <<EOF
ls | grep dshlib.c | wc -l
EOF

    # Assertions
    [ "$status" -eq 0 ] # Exit status should be 0
    [[ "${lines[0]}" == "1" ]] # Output should be "1" (one line of output)
}

# Check cd changes directory
@test "check cd changes directory" {
    current=$(pwd)

    cd /tmp
    mkdir -p dsh-test

    run "${current}/dsh" <<EOF
cd dsh-test
pwd
EOF
    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="/tmp/dsh-testlocalmodedsh4>dsh4>dsh4>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}

# Check built-in command `exit` terminates the shell
@test "check exit terminates the shell" {
    run ./dsh <<EOF
exit
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="localmodedsh4>exiting...cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}


# Check handling of empty input
@test "check empty input is handled properly" {
    run ./dsh <<EOF


EOF

    # Assertions
    [ "$status" -eq 0 ] # Exit status should be 0
    [[ "${output}" == *"warning: no commands provided"* ]] # Warning message should appear
}

@test "Which which ... which?" {
    run "./dsh" <<EOF                
which which
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="/usr/bin/whichlocalmodedsh4>dsh4>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}

@test "It handles quoted spaces" {
    run "./dsh" <<EOF                
   echo " hello     world     " 
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')

    # Expected output with all whitespace removed for easier matching
    expected_output=" hello     world     local modedsh4> dsh4> cmd loop returned 0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}

@test "Change directory - no args" {
    current=$(pwd)

    cd /tmp
    mkdir -p dsh-test

    run "${current}/dsh" <<EOF                
cd
pwd
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="/tmplocalmodedsh4>dsh4>dsh4>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}

@test "try too many commands" {
run ./dsh <<EOF
ls | ls | ls | ls | ls | ls | ls | ls | ls
exit
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    echo "Output: $stripped_output"
    expected_output="error:pipinglimitedto8commandslocalmodedsh4>dsh4>exiting...cmdloopreturned0"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}

# EXTRA CREDIT
@test "Output redirection" {
    run ./dsh <<EOF
echo "hello, class" > out.txt
EOF
    [ "$status" -eq 0 ]
    run cat out.txt
    [[ "${output}" == "hello, class" ]]
}

@test "Combined redirection" {
    echo "class" > in.txt
    run ./dsh <<EOF
grep "class" < in.txt > out.txt
EOF
    [ "$status" -eq 0 ]
    run cat out.txt
    [[ "${output}" == "class" ]]
}

# New Tests
start_remote_server() {
    port=7979
    ./dsh -s -p $port > remote_server.log 2>&1 &
    echo $!
}

@test "remote: echo command works correctly" {
    port=7979
    ./dsh -s -p $port > remote_server.log 2>&1 &
    server_pid=$!
    sleep 1
    run ./dsh -c -p $port <<EOF
echo Remote Test
exit
EOF
    kill $server_pid 2>/dev/null
    # Expect the output (ignoring prompt banners) to contain "Remote Test"
    expected_substring="Remote Test"
    if [[ "$output" != *"$expected_substring"* ]]; then
        echo "Expected output to contain: '$expected_substring'"
        echo "Actual output: '$output'"
        false
    fi
}

@test "remote: pipeline command works correctly" {
    port=7979
    ./dsh -s -p $port > remote_server.log 2>&1 &
    server_pid=$!
    sleep 1
    run ./dsh -c -p $port <<EOF
echo one | wc -w
exit
EOF
    kill $server_pid 2>/dev/null
    # "echo one" produces one word; expect output to contain "1"
    expected_substring="1"
    if [[ "$output" != *"$expected_substring"* ]]; then
        echo "Expected output to contain: '$expected_substring'"
        echo "Actual output: '$output'"
        false
    fi
}

@test "remote: cd command changes directory correctly" {
    port=7979
    ./dsh -s -p $port > remote_server.log 2>&1 &
    server_pid=$!
    sleep 1
    tmp_dir="/tmp/dsh-remote-test"
    mkdir -p $tmp_dir
    run ./dsh -c -p $port <<EOF
cd $tmp_dir
pwd
exit
EOF
    kill $server_pid 2>/dev/null
    if [[ "$output" != *"$tmp_dir"* ]]; then
        echo "Expected output to contain directory: '$tmp_dir'"
        echo "Actual output: '$output'"
        false
    fi
}

@test "remote: handles quoted spaces correctly" {
    port=7979
    ./dsh -s -p $port > remote_server.log 2>&1 &
    server_pid=$!
    sleep 1
    run ./dsh -c -p $port <<EOF
echo "   spaced   text   "
exit
EOF
    kill $server_pid 2>/dev/null
    expected_substring="   spaced   text   "
    if [[ "$output" != *"$expected_substring"* ]]; then
        echo "Expected output to contain: '$expected_substring'"
        echo "Actual output: '$output'"
        false
    fi
}


@test "remote: stop-server command shuts down the server" {
    port=7979
    ./dsh -s -p $port > remote_server.log 2>&1 &
    server_pid=$!
    sleep 1
    run ./dsh -c -p $port <<EOF
stop-server
EOF
    sleep 1
    if kill -0 $server_pid 2>/dev/null; then
       echo "Expected server (PID $server_pid) to be stopped after stop-server command."
       kill $server_pid 2>/dev/null
       false
    fi
}

@test "Remote: exit command terminates client session" {
    port=7979
    server_pid=$(start_remote_server)
    sleep 1
    run ./dsh -c -p $port <<EOF
echo Before Exit
exit
echo After Exit
EOF
    kill $server_pid 2>/dev/null
    if [[ "$output" == "After Exit" ]]; then
        echo "Expected that 'After Exit' does not appear in output"
        echo "Actual output: '$output'"
        false
    fi
}