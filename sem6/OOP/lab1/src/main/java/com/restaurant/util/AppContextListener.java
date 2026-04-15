package com.restaurant.util;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import jakarta.servlet.ServletContextEvent;
import jakarta.servlet.ServletContextListener;
import jakarta.servlet.annotation.WebListener;

/**
 * Initialize DB connection and run migrations (runs by tomcat)
 */
@WebListener
public class AppContextListener implements ServletContextListener {

    private static final Logger logger = LogManager.getLogger(AppContextListener.class);

    @Override
    public void contextInitialized(ServletContextEvent sce) {
        logger.info("Application starting...");

        FlywayMigrationRunner.run();

        DatabaseConnection.getInstance();

        logger.info("Application started successfully");
    }

    @Override
    public void contextDestroyed(ServletContextEvent sce) {
        logger.info("Application shutting down...");
    }
}