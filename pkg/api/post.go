package api

import "github.com/gin-gonic/gin"

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

// PostFeedGet Get /post/feed
func (i *Instance) PostFeedGet(c *gin.Context) {
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
