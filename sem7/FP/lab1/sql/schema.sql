CREATE DATABASE IF NOT EXISTS display_class_schedule
    CHARACTER SET utf8mb4
    COLLATE utf8mb4_unicode_ci;

USE display_class_schedule;

-- Computer classrooms
CREATE TABLE IF NOT EXISTS display_classes (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    room_number VARCHAR(20) NOT NULL UNIQUE,
    capacity INT NOT NULL CHECK (capacity > 0),
    opening_time TIME NOT NULL,
    closing_time TIME NOT NULL,
    CHECK (closing_time > opening_time)
);

-- Individual computers in a classroom
CREATE TABLE IF NOT EXISTS workstations (
    id INT AUTO_INCREMENT PRIMARY KEY,
    class_id INT NOT NULL,
    workstation_number VARCHAR(20) NOT NULL,
    status VARCHAR(20) NOT NULL CHECK (status IN ('available', 'maintenance', 'broken')),
    UNIQUE (class_id, workstation_number),
    FOREIGN KEY (class_id) REFERENCES display_classes(id)
);

-- Students and other computer users
CREATE TABLE IF NOT EXISTS users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    full_name VARCHAR(100) NOT NULL,
    user_type VARCHAR(30) NOT NULL CHECK (user_type IN ('student', 'teacher')),
    email VARCHAR(100) UNIQUE
);

-- Teachers who conduct scheduled classes
CREATE TABLE IF NOT EXISTS teachers (
    id INT AUTO_INCREMENT PRIMARY KEY,
    full_name VARCHAR(100) NOT NULL,
    department VARCHAR(100) NOT NULL,
    email VARCHAR(100) UNIQUE
);

-- University subjects
CREATE TABLE IF NOT EXISTS subjects (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL UNIQUE
);

-- Planned lessons in computer classrooms
CREATE TABLE IF NOT EXISTS scheduled_classes (
    id INT AUTO_INCREMENT PRIMARY KEY,
    class_id INT NOT NULL,
    teacher_id INT NOT NULL,
    subject_id INT NOT NULL,
    group_name VARCHAR(30) NOT NULL,
    lesson_date DATE NOT NULL,
    start_time TIME NOT NULL,
    end_time TIME NOT NULL,
    CHECK (end_time > start_time),
    FOREIGN KEY (class_id) REFERENCES display_classes(id),
    FOREIGN KEY (teacher_id) REFERENCES teachers(id),
    FOREIGN KEY (subject_id) REFERENCES subjects(id)
);

-- Time periods available for independent work
CREATE TABLE IF NOT EXISTS free_access (
    id INT AUTO_INCREMENT PRIMARY KEY,
    class_id INT NOT NULL,
    access_date DATE NOT NULL,
    start_time TIME NOT NULL,
    end_time TIME NOT NULL,
    CHECK (end_time > start_time),
    FOREIGN KEY (class_id) REFERENCES display_classes(id)
);

-- Actual use of individual computers
CREATE TABLE IF NOT EXISTS workstation_usage (
    id INT AUTO_INCREMENT PRIMARY KEY,
    workstation_id INT NOT NULL,
    user_id INT NOT NULL,
    started_at DATETIME NOT NULL,
    ended_at DATETIME,
    CHECK (ended_at IS NULL OR ended_at > started_at),
    FOREIGN KEY (workstation_id) REFERENCES workstations(id),
    FOREIGN KEY (user_id) REFERENCES users(id)
);
