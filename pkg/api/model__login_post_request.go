/*
 * OTUS Highload Architect
 *
 * No description provided (generated by Openapi Generator https://github.com/openapitools/openapi-generator)
 *
 * API version: 1.2.0
 * Generated by: OpenAPI Generator (https://openapi-generator.tech)
 */

package api

type LoginPostRequest struct {
	// Идентификатор пользователя
	Id string `json:"id,omitempty"`

	Password string `json:"password,omitempty"`
}