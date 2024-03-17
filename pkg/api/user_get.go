package api

import (
	"context"
	"net/http"

	"github.com/gin-gonic/gin"
	"github.com/sirupsen/logrus"
)

func (i *Instance) get(ctx context.Context, id string) (*User, error) {
	c := i.db.GetConn()

	row := c.QueryRow(
		ctx,
		"SELECT id, first_name, second_name, birthdate, biography, city "+
			"FROM postgres.public.\"User\" WHERE id=$1", id)

	var user User

	if err := row.Scan(
		&user.Id,
		&user.FirstName,
		&user.SecondName,
		&user.Birthdate,
		&user.Biography,
		&user.City,
	); err != nil {
		return nil, err
	}

	return &user, nil
}

// UserGetIdGet Get /user/get/:id
func (i *Instance) UserGetIdGet(c *gin.Context) {
	id := c.Param("id")

	user, err := i.get(context.Background(), id)
	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"status": "Internal Server Error"})
		logrus.Errorf("Internal Server Error: %v", err)

		return
	}

	c.JSON(http.StatusOK, user)
}
