package api

import (
	"fmt"
	"net/http"
	"strings"

	"github.com/gin-gonic/gin"
	"github.com/golang-jwt/jwt/v5"
	"github.com/sirupsen/logrus"
)

func extractClaims(tokenStr string) (jwt.MapClaims, bool) {
	s := strings.Split(tokenStr, " ")

	hmacSecret := []byte(jwtSignKey)

	token, err := jwt.Parse(s[1], func(token *jwt.Token) (interface{}, error) {
		// check token signing method etc
		return hmacSecret, nil
	})
	if err != nil {
		return nil, false
	}

	if claims, ok := token.Claims.(jwt.MapClaims); ok && token.Valid {
		return claims, true
	} else {
		logrus.Printf("Invalid JWT Token")
		return nil, false
	}
}

func getUserIDFromHeader(c *gin.Context) (string, error) {
	auth := c.Request.Header.Get("Authorization")

	claims, ok := extractClaims(auth)
	if !ok {
		c.JSON(http.StatusBadRequest, gin.H{"status": "Bad request"})
	}

	userID, ok := claims["user_id"]

	if !ok {
		return "", fmt.Errorf("error get user from request")
	}

	result, ok := userID.(string)
	if !ok {
		return "", fmt.Errorf("error get user from request")
	}

	return result, nil
}
