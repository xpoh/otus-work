package api

import (
	"context"
	"encoding/json"
	"errors"
	"fmt"
	"github.com/google/uuid"
	"github.com/segmentio/kafka-go"
	"net/http"
	"time"

	"github.com/gin-gonic/gin"
	"github.com/redis/go-redis/v9"
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

func (i *Instance) friendsList(ctx context.Context, id string) ([]string, error) {
	c := i.db.GetConn()

	rows, err := c.Query(
		ctx,
		`select friend_id from "Friend" where user_id=$1`, id)
	if err != nil {
		return nil, fmt.Errorf("error get feed: %w", err)
	}

	defer rows.Close()

	fiendsID := make([]string, 0)

	for rows.Next() {
		var id string

		if err = rows.Scan(&id); err != nil {
			return nil, fmt.Errorf("error querying post: %w", err)
		}

		fiendsID = append(fiendsID, id)
	}

	return fiendsID, nil
}

func (i *Instance) cacheFeed(ctx context.Context, id, limit, offset string) (string, error) {
	r := i.rds

	key := id + ":" + offset + ":" + limit

	return r.Get(ctx, key).Result()
}

func (i *Instance) cacheSet(ctx context.Context, id, limit, offset string, value string) error {
	r := i.rds

	key := id + ":" + offset + ":" + limit

	return r.Set(ctx, key, value, 10*time.Minute).Err()
}

// PostFeedGet Get /post/feed
func (i *Instance) PostFeedGet(c *gin.Context) {
	offset := c.Query("offset")
	limit := c.Query("limit")

	userID, err := getUserIDFromHeader(c)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"status": err})
	}

	result, err := i.cacheFeed(c, userID, limit, offset)
	if !errors.Is(err, redis.Nil) && err != nil {
		logrus.Errorf("error getting work with cache: %v", err)

		c.JSON(http.StatusInternalServerError, gin.H{"status:": err.Error()})
	}

	if err == nil {
		c.JSON(http.StatusOK, result)
	}

	posts, err := i.feed(context.Background(), userID, limit, offset)
	if err != nil {
		logrus.Errorf("error getting posts: %v", err)

		c.JSON(http.StatusNotFound, gin.H{"status": "not find"})
	}

	p, err := json.Marshal(posts)
	if err != nil {
		logrus.Errorf("error marshalling posts: %v", err)
	}

	if err := i.cacheSet(c, userID, limit, offset, string(p)); err != nil {
		logrus.Errorf("error cache set: %v", err)
	}

	c.JSON(http.StatusOK, posts)
}

func (i *Instance) postCreate(ctx context.Context, userID, text string) error {
	c := i.db.GetConn()

	newID := uuid.New().String()

	if _, err := c.Exec(
		ctx,
		"INSERT INTO postgres.public.\"Post\" VALUES ($1,$2,$3)",
		newID,
		text,
		userID,
	); err != nil {
		return err
	}

	if err := i.notifyFriends(ctx, newID, userID, text); err != nil {
		logrus.Errorf("error notification %v [%s]: %v", userID, text, err)
	}

	return nil
}

func (i *Instance) notifyFriends(ctx context.Context, postID, userID, text string) error {
	friendsID, err := i.friendsList(ctx, userID)
	if err != nil {
		return err
	}

	kafkaClient := kafka.Writer{
		Addr:                   kafka.TCP(i.cfg.GetKafkaBrokers()...),
		AllowAutoTopicCreation: true,
	}

	for _, id := range friendsID {
		message := models.Post{
			Id:           postID,
			Text:         text,
			AuthorUserId: userID,
		}.String()

		if err := kafkaClient.WriteMessages(ctx, kafka.Message{
			Topic: id,
			Value: []byte(message),
		}); err != nil {
			return err
		}

		logrus.Infof(message)
	}

	return nil
}

// PostCreatePost Post /post/create
func (i *Instance) PostCreatePost(c *gin.Context) {
	userID, err := getUserIDFromHeader(c)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"status": err})
	}

	request := models.PostCreatePostRequest{}

	if err := c.Bind(&request); err != nil {
		c.JSON(400, gin.H{"status": "Невалидные данные"})
		logrus.Debugf("Невалидные данные: %v", c.Request)

		return
	}

	if err := i.postCreate(context.Background(), userID, request.Text); err != nil {
		logrus.Errorf("error create post: %v", err)

		c.JSON(http.StatusNotFound, gin.H{"status": "error create post"})
	}

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
