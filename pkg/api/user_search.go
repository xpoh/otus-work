package api

import (
	"context"
	"fmt"
	"net/http"

	"github.com/gin-gonic/gin"
	"github.com/sirupsen/logrus"
	"github.com/xpoh/otus-work/pkg/api/models"
)

func (i *Instance) search(ctx context.Context, first, last string) ([]models.User, error) {
	c := i.db.GetConn()

	rows, err := c.Query(
		ctx,
		"SELECT id, first_name, second_name, birthdate, biography, city "+
			"FROM postgres.public.\"User\" "+
			"WHERE to_tsvector('english', first_name || ' ' || second_name) @@ to_tsquery('english', $1) AND "+
			"to_tsvector('english', first_name || ' ' || second_name) @@ to_tsquery('english', $2) "+
			"ORDER BY id",
		fmt.Sprintf("%s%%", first),
		fmt.Sprintf("%s%%", last),
	)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	users := make([]models.User, 0)

	for rows.Next() {
		var user models.User
		if err := rows.Scan(
			&user.Id,
			&user.FirstName,
			&user.SecondName,
			&user.Birthdate,
			&user.Biography,
			&user.City,
		); err != nil {
			return nil, err
		}

		users = append(users, user)
	}

	return users, nil
}

// UserSearchGet Get /user/search
func (i *Instance) UserSearchGet(c *gin.Context) {
	firstName := c.Query("first_name")
	lastName := c.Query("last_name")

	users, err := i.search(c, firstName, lastName)
	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"status": "Internal Server Error"})
		logrus.Errorf("Internal Server Error: %v", err)

		return
	}

	c.JSON(http.StatusOK, users)
}
