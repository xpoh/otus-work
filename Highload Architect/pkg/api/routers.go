/*
 * OTUS Highload Architect
 *
 * No description provided (generated by Openapi Generator https://github.com/openapitools/openapi-generator)
 *
 * API version: 1.2.0
 * Generated by: OpenAPI Generator (https://openapi-generator.tech)
 */

package api

import (
	"net/http"

	"github.com/gin-gonic/gin"
)

// Route is the information for every URI.
type Route struct {
	// Name is the name of this Route.
	Name string
	// Method is the string for the HTTP method. ex) GET, POST etc..
	Method string
	// Pattern is the pattern of the URI.
	Pattern string
	// HandlerFunc is the handler function of this route.
	HandlerFunc gin.HandlerFunc
}

// NewRouter returns a new router.
func NewRouter(handleFunctions ApiHandleFunctions) *gin.Engine {
	return NewRouterWithGinEngine(gin.Default(), handleFunctions)
}

// NewRouterWithGinEngine NewRouter add routes to existing gin engine.
func NewRouterWithGinEngine(router *gin.Engine, handleFunctions ApiHandleFunctions) *gin.Engine {
	for _, route := range getRoutes(handleFunctions) {
		if route.HandlerFunc == nil {
			route.HandlerFunc = DefaultHandleFunc
		}

		switch route.Method {
		case http.MethodGet:
			router.GET(route.Pattern, route.HandlerFunc)
		case http.MethodPost:
			router.POST(route.Pattern, route.HandlerFunc)
		case http.MethodPut:
			router.PUT(route.Pattern, route.HandlerFunc)
		case http.MethodPatch:
			router.PATCH(route.Pattern, route.HandlerFunc)
		case http.MethodDelete:
			router.DELETE(route.Pattern, route.HandlerFunc)
		}
	}

	return router
}

// DefaultHandleFunc Default handler for not yet implemented routes
func DefaultHandleFunc(c *gin.Context) {
	c.String(http.StatusNotImplemented, "501 not implemented")
}

type ApiHandleFunctions struct {
	// Routes for the DefaultAPI part of the API
	DefaultAPI *Instance
}

func getRoutes(handleFunctions ApiHandleFunctions) []Route {
	return []Route{
		{
			"DialogUserIdListGet",
			http.MethodGet,
			"/dialog/:user_id/list",
			handleFunctions.DefaultAPI.DialogUserIdListGet,
		},
		{
			"DialogUserIdSendPost",
			http.MethodPost,
			"/dialog/:user_id/send",
			handleFunctions.DefaultAPI.DialogUserIdSendPost,
		},
		{
			"DialogUserIdListGetV2",
			http.MethodGet,
			"/v2/dialog/:user_id/list",
			handleFunctions.DefaultAPI.DialogUserIdListGetV2,
		},
		{
			"DialogUserIdSendPostV2",
			http.MethodPost,
			"/v2/dialog/:user_id/send",
			handleFunctions.DefaultAPI.DialogUserIdSendPostV2,
		},
		{
			"FriendDeleteUserIdPut",
			http.MethodPut,
			"/friend/delete/:user_id",
			handleFunctions.DefaultAPI.FriendDeleteUserIdPut,
		},
		{
			"FriendSetUserIdPut",
			http.MethodPut,
			"/friend/set/:user_id",
			handleFunctions.DefaultAPI.FriendSetUserIdPut,
		},
		{
			"LoginPost",
			http.MethodPost,
			"/login",
			handleFunctions.DefaultAPI.LoginPost,
		},
		{
			"PostCreatePost",
			http.MethodPost,
			"/post/create",
			handleFunctions.DefaultAPI.PostCreatePost,
		},
		{
			"PostDeleteIdPut",
			http.MethodPut,
			"/post/delete/:id",
			handleFunctions.DefaultAPI.PostDeleteIdPut,
		},
		{
			"PostFeedGet",
			http.MethodGet,
			"/post/feed",
			handleFunctions.DefaultAPI.PostFeedGet,
		},
		{
			"PostGetIdGet",
			http.MethodGet,
			"/post/get/:id",
			handleFunctions.DefaultAPI.PostGetIdGet,
		},
		{
			"PostUpdatePut",
			http.MethodPut,
			"/post/update",
			handleFunctions.DefaultAPI.PostUpdatePut,
		},
		{
			"UserGetIdGet",
			http.MethodGet,
			"/user/get/:id",
			handleFunctions.DefaultAPI.UserGetIdGet,
		},
		{
			"UserRegisterPost",
			http.MethodPost,
			"/user/register",
			handleFunctions.DefaultAPI.UserRegisterPost,
		},
		{
			"UserSearchGet",
			http.MethodGet,
			"/user/search",
			handleFunctions.DefaultAPI.UserSearchGet,
		},
		{
			"Websocket",
			http.MethodGet,
			"/post/feed/posted/:id",
			handleFunctions.DefaultAPI.WsHandler,
		},
		{
			"Index",
			http.MethodGet,
			"/",
			handleFunctions.DefaultAPI.Index,
		},
	}
}
