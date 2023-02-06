/*
Copyright 2021 The Kubernetes Authors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

package serde

import (
	"testing"

	"sigs.k8s.io/kpng/api/localv1"
)

func TestHashIsStable(t *testing.T) {
	ep := &localv1.Endpoint{}
	ep.PortOverrides = []*localv1.PortName{
		{Name: "a", Port: 1},
		{Name: "b", Port: 2},
		{Name: "c", Port: 3},
	}

	ref := Hash(ep)
	for i := 0; i < 100; i++ {
		h := Hash(ep)
		if ref != h {
			t.Errorf("hash is not stable: %x != %x", ref, h)
		}
	}
}
