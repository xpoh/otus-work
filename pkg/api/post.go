package api

import (
	"context"
	"fmt"
	"net/http"

	"github.com/gin-gonic/gin"
	"github.com/sirupsen/logrus"
	"github.com/xpoh/otus-work/pkg/api/models"
)

func (i *Instance) feed(ctx context.Context, id string, limit, offset string) ([]*models.Post, error) {
	c := i.db.GetConn()

	rows, err := c.Query(
		ctx,
		`with fr as (select friend_id from "Friend" where user_id=$1)
				select id, text, author_user_id from "Post", fr
				where author_user_id = fr.friend_id
				LIMIT $2
				OFFSET $3`, id, limit, offset)

	if err != nil {
		return nil, fmt.Errorf("error get feed: %w", err)
	}

	defer rows.Close()

	posts := make([]*models.Post, 0)

	for rows.Next() {
		var post models.Post

		if err = rows.Scan(
			&post.Id,
			&post.Text,
			&post.AuthorUserId,
		); err != nil {
			return nil, fmt.Errorf("error querying post: %w", err)
		}

		posts = append(posts, &post)
	}

	return posts, nil
}

// PostFeedGet Get /post/feed
func (i *Instance) PostFeedGet(c *gin.Context) {
	offset := c.Query("offset")
	limit := c.Query("limit")

	userID, err := getUserIDFromHeader(c)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"status": err})
	}

	posts, err := i.feed(context.Background(), userID, limit, offset)
	if err != nil {
		logrus.Errorf("error getting posts: %v", err)

		c.JSON(http.StatusNotFound, gin.H{"status": "not find"})
	}

	c.JSON(http.StatusOK, posts)
}

// PostCreatePost Post /post/create
func (i *Instance) PostCreatePost(c *gin.Context) {
	// Your handler implementation
	c.JSON(200, gin.H{"status": "OK"})
}

// PostDeleteIdPut Put /post/delete/:id
func (i *Instance) PostDeleteIdPut(c *gin.Context) {
	// Your handler implementation
	c.JSON(200, gin.H{"status": "OK"})
}

// PostGetIdGet Get /post/get/:id
func (i *Instance) PostGetIdGet(c *gin.Context) {
	// Your handler implementation
	c.JSON(200, gin.H{"status": "OK"})
}

// PostUpdatePut Put /post/update
func (i *Instance) PostUpdatePut(c *gin.Context) {
	// Your handler implementation
	c.JSON(200, gin.H{"status": "OK"})
}
