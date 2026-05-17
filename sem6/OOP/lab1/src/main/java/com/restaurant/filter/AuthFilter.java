package com.restaurant.filter;

import java.io.IOException;
import java.net.URL;
import java.security.interfaces.RSAPublicKey;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.auth0.jwk.JwkProvider;
import com.auth0.jwk.JwkProviderBuilder;
import com.auth0.jwt.JWT;
import com.auth0.jwt.algorithms.Algorithm;
import com.auth0.jwt.interfaces.DecodedJWT;
import com.restaurant.model.entity.User;
import com.restaurant.service.UserService;

import io.github.cdimascio.dotenv.Dotenv;
import jakarta.servlet.Filter;
import jakarta.servlet.FilterChain;
import jakarta.servlet.FilterConfig;
import jakarta.servlet.ServletException;
import jakarta.servlet.ServletRequest;
import jakarta.servlet.ServletResponse;
import jakarta.servlet.annotation.WebFilter;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;

@WebFilter("/*")
public class AuthFilter implements Filter {

    private static final Logger logger = LogManager.getLogger(AuthFilter.class);

    private String domain;
    private String audience;
    private JwkProvider jwkProvider;
    private UserService userService;

    @Override
    public void init(FilterConfig filterConfig) {
        Dotenv dotenv = Dotenv.load();
        this.domain = dotenv.get("AUTH0_DOMAIN");
        this.audience = dotenv.get("AUTH0_AUDIENCE");
        this.userService = new UserService();

        try {
            this.jwkProvider = new JwkProviderBuilder(new URL("https://" + domain))
                    .build();
            logger.info("AuthFilter initialized with domain: {}", domain);
        } catch (Exception e) {
            logger.error("Failed to initialize AuthFilter", e);
            throw new RuntimeException("AuthFilter initialization failed", e);
        }
    }

    @Override
    public void doFilter(ServletRequest request, ServletResponse response,
            FilterChain chain) throws IOException, ServletException {

        HttpServletRequest httpRequest = (HttpServletRequest) request;
        HttpServletResponse httpResponse = (HttpServletResponse) response;

        String authHeader = httpRequest.getHeader("Authorization");

        if (authHeader == null || !authHeader.startsWith("Bearer ")) {
            sendUnauthorized(httpResponse, "Missing or invalid Authorization header");
            return;
        }

        String token = authHeader.substring(7);

        try {
            DecodedJWT decodedJWT = verifyToken(token);
            String auth0Id = decodedJWT.getSubject();

            String name = decodedJWT.getClaim("name").asString();
            String email = decodedJWT.getClaim("email").asString();

            User user = userService.findOrCreate(auth0Id, name, email);

            httpRequest.setAttribute("currentUser", user);

            logger.debug("Authenticated user: {}", email);
            chain.doFilter(request, response);

        } catch (Exception e) {
            logger.warn("JWT verification failed: {}", e.getMessage());
            sendUnauthorized(httpResponse, "Invalid or expired token");
        }
    }

    private DecodedJWT verifyToken(String token) throws Exception {

        DecodedJWT unverified = JWT.decode(token);
        String keyId = unverified.getKeyId();

        RSAPublicKey publicKey = (RSAPublicKey) jwkProvider.get(keyId).getPublicKey();

        Algorithm algorithm = Algorithm.RSA256(publicKey, null);

        return JWT.require(algorithm)
                .withIssuer("https://" + domain + "/")
                .withAudience(audience)
                .build()
                .verify(token);
    }

    private void sendUnauthorized(HttpServletResponse response, String message) throws IOException {
        response.setStatus(HttpServletResponse.SC_UNAUTHORIZED);
        response.setContentType("application/json");
        response.setCharacterEncoding("UTF-8");
        response.getWriter().write("{\"error\": \"" + message + "\"}");
    }

    @Override
    public void destroy() {
        logger.info("AuthFilter destroyed");
    }
}