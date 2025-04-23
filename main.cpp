#include <iostream>
#include <cstdlib>
#include <cmath>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <sys/select.h>

using namespace std;

const int numofRobots = 50;
const int roomsiz = 100;
const int exit_minwidth = 16;
const int exit_maxwidth = 26;
const int threshold = 5;
const int max_dist = 10;
const int neighbor_dist = 5;

struct RobotData {
    int id;
    int position_x;
    int position_y;
    int pipe_fd[2];  // Pipe file descriptors for IPC
    int distance_to_exit; // Distance of the robot from the exit
};

// Declare utility functions
double calculateAccuracy(int distance);
bool areNeighbors(const RobotData& robot1, const RobotData& robot2);

// Declare robot_process function
void robot_process(RobotData& robot, int* shared_memory, const std::vector<RobotData>& all_robots);

// Define utility functions
double calculateAccuracy(int distance) {
    if (distance <= threshold) {
        return 0.95;  // Accuracy 95%
    } else if (distance <= max_dist) {
        return 0.88 + (distance - threshold) * 0.01;  // Accuracy 88-95%
    } else {
        return 0.0;  
    }
}

bool areNeighbors(const RobotData& robot1, const RobotData& robot2) {
    int dist_x = robot1.position_x - robot2.position_x;
    int dist_y = robot1.position_y - robot2.position_y;
    return sqrt(dist_x * dist_x + dist_y * dist_y) <= neighbor_dist;
}

// Define robot_process function
void robot_process(RobotData& robot, int* shared_memory, const std::vector<RobotData>& all_robots) {
    srand(time(NULL) + robot.id); // Seed the random number generator

    // Initialize robot's position
    robot.position_x = rand() % roomsiz;
    robot.position_y = rand() % roomsiz;

    // Calculate distance to the exit
    robot.distance_to_exit = std::abs(robot.position_x - roomsiz / 2);

    // Estimate exit width
    double accuracy = calculateAccuracy(robot.distance_to_exit);
    int estimated_width = static_cast<int>((exit_maxwidth - exit_minwidth + 1) * accuracy) + exit_minwidth;

    // Calculate neighbors
    std::vector<int> neighbors;
    for (const auto& other_robot : all_robots) {
        if (areNeighbors(robot, other_robot) && robot.id != other_robot.id) {
            neighbors.push_back(other_robot.id);
        }
    }

    // Read estimates from neighbors with timeout
    int sum_widths = 0;
    int num_neighbors = 0;
    struct timeval timeout;
    timeout.tv_sec = 10; // Set timeout to 10 seconds
    timeout.tv_usec = 0;

    fd_set read_fds;
    for (int neighbor_id : neighbors) {
        FD_SET(all_robots[neighbor_id].pipe_fd[0], &read_fds);
    }

    int result = select(FD_SETSIZE, &read_fds, nullptr, nullptr, &timeout);

    if (result > 0) {
        // Data available from at least one neighbor
        for (int neighbor_id : neighbors) {
            if (FD_ISSET(all_robots[neighbor_id].pipe_fd[0], &read_fds)) {
                int neighbor_estimate;
                read(all_robots[neighbor_id].pipe_fd[0], &neighbor_estimate, sizeof(neighbor_estimate));
                sum_widths += neighbor_estimate;
                num_neighbors++;
            }
        }
    } else {
        sum_widths += estimated_width;
        num_neighbors++;
    }

    // Update shared memory if neighbors available
    if (num_neighbors > 0) {
        int average_width = sum_widths / num_neighbors;
        shared_memory[robot.id] = average_width;
    } else {
        shared_memory[robot.id] = estimated_width; // Use own estimate if no neighbors
    }

    // Display width distance from exit for this robot
    cout << "Robot " << robot.id << " Distance to Exit: " << robot.distance_to_exit << " Estimated Width: " << shared_memory[robot.id] << " True Width: " << (exit_minwidth + exit_maxwidth) / 2 << endl;
}

// Main program
int main() {

    cout << "OS PROJECT STARTED BY IBRAHIM and HAMZA ... " << endl;

    int shmid = shmget(IPC_PRIVATE, numofRobots * sizeof(int), IPC_CREAT | 0666);
    int* shared_memory = (int*)shmat(shmid, nullptr, 0);

    std::vector<RobotData> robots(numofRobots);

    // Create pipes for IPC and fork processes for each robot
    for (int i = 0; i < numofRobots; ++i) {
        pipe(robots[i].pipe_fd);
        robots[i].id = i;
        pid_t pid = fork();
        if (pid == 0) { // Child process
            robot_process(robots[i], shared_memory, robots);
            exit(0);
        }
    }

    // Wait for all child processes to complete
    for (int i = 0; i < numofRobots; ++i) {
        wait(nullptr);
    }

    // Calculate and display final results
    int total_width = 0;
    for (int i = 0; i < numofRobots; ++i) {
        total_width += shared_memory[i];
    }
    int average_width = total_width / numofRobots;
    int true_width = (exit_minwidth + exit_maxwidth) / 2;

    cout << "Estimated Exit Width: " << average_width << endl;
    cout << "True Exit Width: " << true_width << endl;
    cout << "Difference: " << std::abs(average_width - true_width) << endl;

    // Cleanup
    shmdt(shared_memory);
    shmctl(shmid, IPC_RMID, nullptr);
    for (const auto& robot : robots) {
        close(robot.pipe_fd[0]);
        close(robot.pipe_fd[1]);
    }

    return 0;
}
