/*
 * OTUS Highload Architect
 *
 * No description provided (generated by Openapi Generator https://github.com/openapitools/openapi-generator)
 *
 * API version: 1.2.0
 * Generated by: OpenAPI Generator (https://openapi-generator.tech)
 */

package models

// Post - Пост пользователя
type Post struct {
	// Идентификатор поста
	Id string `json:"id,omitempty"`

	// Текст поста
	Text string `json:"text,omitempty"`

	// Идентификатор пользователя
	AuthorUserId string `json:"author_user_id,omitempty"`
}