package api

import "testing"

func Test_passHash(t *testing.T) {
	type args struct {
		pass string
	}
	tests := []struct {
		name string
		args args
		want string
	}{
		{
			name: "passHash",
			args: args{pass: "123456"},
			want: "ba3253876aed6bc22d4a6ff53d8406c6ad864195ed144ab5c87621b6c233b548baeae6956df346ec8c17f5ea10f35ee3cbc514797ed7ddd3145464e2a0bab413",
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := passHash(tt.args.pass); got != tt.want {
				t.Errorf("passHash() = %v, want %v", got, tt.want)
			}
		})
	}
}
