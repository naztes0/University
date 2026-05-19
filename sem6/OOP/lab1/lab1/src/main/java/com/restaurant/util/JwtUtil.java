package com.restaurant.util;

import com.auth0.jwt.JWT;
import com.auth0.jwt.algorithms.Algorithm;
import com.auth0.jwt.exceptions.JWTVerificationException;
import com.auth0.jwt.interfaces.DecodedJWT;
import io.github.cdimascio.dotenv.Dotenv;

import java.util.Date;

/**
 * Utility class for generating and validating JWT tokens.
 */
public class JwtUtil {

    private static final long EXPIRATION_MS = 24 * 60 * 60 * 1000; 
    private static final String ISSUER = "restaurant-app";
    private static final Algorithm ALGORITHM;
    
    static {
        Dotenv dotenv = Dotenv.configure()
                .directory(System.getProperty("user.dir"))
                .ignoreIfMissing()
                .load();
        String secret = dotenv.get("JWT_SECRET");
        ALGORITHM = Algorithm.HMAC256(secret);
    }

    /**
     * Generates a JWT token for a user.
     */
    public static String generateToken(Long userId, String email, String role) {
        return JWT.create()
                .withIssuer(ISSUER)
                .withSubject(String.valueOf(userId))
                .withClaim("email", email)
                .withClaim("role", role)
                .withIssuedAt(new Date())
                .withExpiresAt(new Date(System.currentTimeMillis() + EXPIRATION_MS))
                .sign(ALGORITHM);
    }

    /**
     * Validates and decodes a JWT token.
     * Returns decoded token if valid, throws exception if invalid.
     */
    public static DecodedJWT validateToken(String token) throws JWTVerificationException {
        return JWT.require(ALGORITHM)
                .withIssuer(ISSUER)
                .build()
                .verify(token);
    }
}