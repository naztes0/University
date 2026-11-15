USE display_class_schedule;

INSERT IGNORE INTO display_classes
    (id, name, room_number, capacity, opening_time, closing_time)
VALUES
    (1, 'Computer class 1', '305', 20, '08:00:00', '18:00:00'),
    (2, 'Computer class 2', '307', 15, '09:00:00', '17:00:00');

INSERT IGNORE INTO workstations
    (id, class_id, workstation_number, status)
VALUES
    (1, 1, 'PC-01', 'available'),
    (2, 1, 'PC-02', 'available'),
    (3, 2, 'PC-01', 'available'),
    (4, 2, 'PC-02', 'maintenance');

INSERT IGNORE INTO users
    (id, full_name, user_type, email)
VALUES
    (1, 'Ivan Petrenko', 'student', 'ivan.petrenko@example.com'),
    (2, 'Maria Kovalenko', 'student', 'maria.kovalenko@example.com');

INSERT IGNORE INTO teachers
    (id, full_name, department, email)
VALUES
    (1, 'Olena Koval', 'Programming Department', 'olena.koval@example.com'),
    (2, 'Andrii Melnyk', 'Information Systems Department', 'andrii.melnyk@example.com');

INSERT IGNORE INTO subjects
    (id, name)
VALUES
    (1, 'Functional Programming'),
    (2, 'Databases');

INSERT IGNORE INTO scheduled_classes
    (id, class_id, teacher_id, subject_id, group_name, lesson_date, start_time, end_time)
VALUES
    (1, 1, 1, 1, 'KN-41', '2026-11-17', '10:00:00', '11:20:00'),
    (2, 2, 2, 2, 'KN-42', '2026-11-18', '12:00:00', '13:20:00');

INSERT IGNORE INTO free_access
    (id, class_id, access_date, start_time, end_time)
VALUES
    (1, 1, '2026-11-17', '14:00:00', '17:00:00'),
    (2, 2, '2026-11-18', '14:00:00', '16:00:00');

INSERT IGNORE INTO workstation_usage
    (id, workstation_id, user_id, started_at, ended_at)
VALUES
    (1, 1, 1, '2026-11-17 14:10:00', '2026-11-17 15:00:00'),
    (2, 3, 2, '2026-11-18 14:20:00', '2026-11-18 15:30:00');
