USE display_class_schedule;

SET FOREIGN_KEY_CHECKS = 0;

INSERT INTO display_classes VALUES
    (1, 'Computer class 1', '305', 20, '08:00:00', '18:00:00'),
    (2, 'Computer class 2', '307', 15, '09:00:00', '17:00:00'),
    (3, 'Computer class 5', '306', 11, '08:00:00', '17:00:00'),
    (6, 'Computer class 4', '304', 10, '00:00:09', '00:00:09');

INSERT INTO subjects VALUES
    (1, 'Functional Programming'),
    (2, 'Databases');

INSERT INTO teachers VALUES
    (1, 'Olena Koval', 'Programming Department', 'olena.koval@example.com'),
    (2, 'Andriy Melnyk', 'Systme Department', 'andrii.melnyk@example1.com'),
    (3, 'test', 'FP', NULL);

INSERT INTO users VALUES
    (1, 'Ivan Petrenko', 'student', 'ivan.petrenko@example.com'),
    (2, 'Maria Kovalenko', 'student', 'maria.kovalenko@example.com'),
    (3, 'Test Testovich', 'stude', NULL);

INSERT INTO workstations VALUES
    (1, 1, 'PC-01', 'available'),
    (2, 1, 'PC-02', 'available'),
    (3, 2, 'PC-01', 'available'),
    (4, 2, 'PC-02', 'maintenance');

INSERT INTO scheduled_classes VALUES
    (1, 1, 1, 1, 'KN-41', '2026-11-17', '10:00:00', '11:20:00'),
    (2, 2, 2, 2, 'KN-42', '2026-11-18', '12:00:00', '13:20:00');

INSERT INTO free_access VALUES
    (1, 1, '2026-11-17', '14:00:00', '17:00:00'),
    (2, 2, '2026-11-18', '14:00:00', '16:00:00');

INSERT INTO workstation_usage VALUES
    (1, 1, 1, '2026-11-17 14:10:00', '2026-11-17 15:00:00'),
    (2, 3, 2, '2026-11-18 14:20:00', '2026-11-18 15:30:00');

SET FOREIGN_KEY_CHECKS = 1;
