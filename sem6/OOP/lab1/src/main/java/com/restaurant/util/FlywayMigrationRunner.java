package com.restaurant.util;

import io.github.cdimascio.dotenv.Dotenv;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.flywaydb.core.Flyway;

/**
 * Runs Flyway migrations on app startup
 */
public class FlywayMigrationRunner {

    private static final Logger logger = LogManager.getLogger(FlywayMigrationRunner.class);

    public static void run() {
        Dotenv dotenv = Dotenv.load();

        logger.info("Starting Flyway migrations...");

        Flyway flyway = Flyway.configure()
                .dataSource(
                        dotenv.get("DB_URL"),
                        dotenv.get("DB_USER"),
                        dotenv.get("DB_PASSWORD"))
                .locations("classpath:db/migration")
                .load();

        flyway.migrate();

        logger.info("Flyway migrations completed successfully");
    }
}