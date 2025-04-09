package api

import (
	"testing"
)

func Test_extractClaims(t *testing.T) {
	type args struct {
		tokenStr string
	}
	tests := []struct {
		name string
		args args
		want bool
	}{
		{
			name: jwtSignKey,
			args: args{tokenStr: "Hello eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJoYXNoIjoiOWFmZTE1ZjkyNTMyYTNiNzQyMGJkMTExMzk2MWU2Y2E4Y2I2NGI4MzQ2Zjk1ZGRmMTZjZTExZjgxZDYwMmFiOTFiNWFmOGIwZDAzZjI3YmRhYzc5MGYzMzJkY2MzZGZjYzU1ZjJiMTQ0ZTU1MzRmMzkyNDgwYjVkNTQ5NDQwZjgiLCJ1c2VyX2lkIjoiOWFhYWUxOTYtMDUxNy00MzU2LTk5NGYtZDY5NmQxZDc2ZTM3In0.G_-iud49cHyuYhh8wWRvYU6lAljhS9BmXjTep-3AWKA"},
			want: true,
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			_, got1 := extractClaims(tt.args.tokenStr)
			if got1 != tt.want {
				t.Errorf("extractClaims() got1 = %v, want %v", got1, tt.want)
			}
		})
	}
}
