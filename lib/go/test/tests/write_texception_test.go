/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

package tests

import (
	"context"
	"errors"
	"testing"

	"github.com/apache/thrift/lib/go/test/gopath/src/unionbinarytest"
	"github.com/apache/thrift/lib/go/thrift"
)

func TestWriteUnionTException(t *testing.T) {
	// See https://issues.apache.org/jira/browse/THRIFT-5845
	s := unionbinarytest.NewSample()
	proto := thrift.NewTBinaryProtocolConf(thrift.NewTMemoryBuffer(), nil)
	err := s.Write(context.Background(), proto)
	t.Log(err)
	if err == nil {
		t.Fatal("Writing empty union did not produce error")
	}
	var te thrift.TException
	if !errors.As(err, &te) {
		t.Fatalf("Error from writing empty union is not TException: (%T) %v", err, err)
	}
	if typ := te.TExceptionType(); typ != thrift.TExceptionTypeProtocol && typ != thrift.TExceptionTypeTransport {
		t.Errorf("Got TExceptionType %v, want one of TProtocolException or TTransportException", typ)
	}
}

func TestWriteSetTException(t *testing.T) {
	// See https://issues.apache.org/jira/browse/THRIFT-5845
	s := unionbinarytest.NewSample()
	s.U4 = []string{
		"foo",
		"foo", // duplicate
	}
	proto := thrift.NewTBinaryProtocolConf(thrift.NewTMemoryBuffer(), nil)
	err := s.Write(context.Background(), proto)
	t.Log(err)
	if err == nil {
		t.Fatal("Writing duplicate set did not produce error")
	}
	var te thrift.TException
	if !errors.As(err, &te) {
		t.Fatalf("Error from writing duplicate set is not TException: (%T) %v", err, err)
	}
	if typ := te.TExceptionType(); typ != thrift.TExceptionTypeProtocol && typ != thrift.TExceptionTypeTransport {
		t.Errorf("Got TExceptionType %v, want one of TProtocolException or TTransportException", typ)
	}
}
