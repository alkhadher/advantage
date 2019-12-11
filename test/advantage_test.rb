#====================================================
#
#    Copyright 2008-2010 iAnywhere Solutions, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#
# See the License for the specific language governing permissions and
# limitations under the License.
#
#
#====================================================

require 'test/unit'
require 'date'

begin
  require 'rubygems'
  unless defined? Advantage
    require 'advantage'
  end
end

class Types
    A_INVALID_TYPE= 0
    A_BINARY      = 1
    A_STRING      = 2
    A_DOUBLE      = 3
    A_VAL64       = 4
    A_UVAL64      = 5
    A_VAL32       = 6
    A_UVAL32      = 7
    A_VAL16       = 8
    A_UVAL16      = 9
    A_VAL8        = 10
    A_UVAL8       = 11
    A_NCHAR       = 12
    A_DECIMAL     = 13
    A_DATE        = 14
    A_TIME        = 15
    A_TIMESTAMP   = 16
end

class Direction
    DD_INVALID       = 0
    DD_INPUT         = 1
    DD_OUTPUT        = 2
    DD_INPUT_OUTPUT  = 3
end

class Advantage_Test < Test::Unit::TestCase
  def setup
    @api = Advantage::AdvantageInterface.new()
    assert_not_nil @api
    assert_nothing_raised do
      Advantage::API.ads_initialize_interface( @api )
    end
    assert_nothing_raised do
      @api.ads_init()
    end
    @conn = @api.ads_new_connection()
    assert_not_nil @conn
    conn_str = "data source=c:\\test\\ruby.add;user id=adssys;DateFormat=YYYY-MM-DD"
    assert_succeeded @api.ads_connect(@conn, conn_str)
  end

  def teardown
    assert_succeeded @api.ads_execute_immediate(@conn, 'SELECT 1 FROM system.iota')
    assert_nil @api.ads_disconnect(@conn)
    assert_failed @api.ads_execute_immediate(@conn, 'SELECT 1 FROM system.iota')
    assert_nil @api.ads_free_connection(@conn)
    assert_nothing_raised do
      @api.ads_fini()
    end
    assert_nothing_raised do
      Advantage::API.ads_finalize_interface( @api )
    end
  end

  def test_execute_immediate
    assert_succeeded @api.ads_execute_immediate(@conn, 'SELECT 1 FROM system.iota')
  end

  #EJS - Need to update the EQUAL, etc.
  def test_errors
    sql = "INSERT INTO test(\"id\") VALUES('test');"
    assert_failed @api.ads_execute_immediate(@conn, sql)
    code, msg = @api.ads_error(@conn)
    assert_equal 7200, code
    assert_not_equal "", msg
    assert_equal "S0000", @api.ads_sqlstate(@conn)
    assert_nil @api.ads_clear_error(@conn)
    code, msg = @api.ads_error(@conn)
    assert_equal 0, code
    assert_equal "", msg
  end

  def test_rollback
    @api.AdsBeginTransaction(@conn)
    id = setup_transaction
    @api.ads_rollback(@conn)
    sql = "SELECT * FROM test where \"id\" = "  + id.to_s + ";"
    rs = exec_direct_with_test(sql)
    assert_failed @api.ads_fetch_next(rs)
  end

  def test_commit
    id = setup_transaction
    @api.ads_commit(@conn)
    sql = "SELECT * FROM test where \"id\" = "  + id.to_s + ";"
    rs = exec_direct_with_test(sql)
    assert_succeeded @api.ads_fetch_next(rs)
    res, ret_id = @api.ads_get_column(rs, 0)
    assert_succeeded res
    assert_not_nil ret_id
    assert_equal id, ret_id
    assert_failed @api.ads_fetch_next(rs)
  end

  def test_column_info
     rs = exec_direct_with_test("SELECT TOP 2 * FROM \"types\" ORDER BY \"id\"")
    assert_equal 12, @api.ads_num_cols(rs)
    assert_column_info(rs, 0, "id", Types::A_VAL32, 4)
    assert_column_info(rs, 1, "_blob_", Types::A_BINARY, 9)
    assert_column_info(rs, 2, "_numeric_", Types::A_DECIMAL, 4)
    assert_column_info(rs, 3, "_char_", Types::A_STRING, 255)
    assert_column_info(rs, 4, "_memo_", Types::A_STRING, 9)
    assert_column_info(rs, 5, "_int_", Types::A_VAL32, 4)
    assert_column_info(rs, 6, "_short_", Types::A_VAL16, 2)
    assert_column_info(rs, 7, "_logical_", Types::A_UVAL8, 1)
    assert_column_info(rs, 8, "_date_", Types::A_DATE, 4)
    assert_column_info(rs, 9, "_timestamp_", Types::A_TIMESTAMP, 8)
    assert_column_info(rs, 10, "_double_", Types::A_DOUBLE, 8)
    assert_nil @api.ads_free_stmt(rs)
  end

  #EJS - Check out return values
  #BLOB, LOGICAL
  def test_bounds_on_types
     rs = exec_direct_with_test("SELECT TOP 2 * FROM \"types\" ORDER BY \"id\"")
    assert_succeeded @api.ads_fetch_next(rs)
    assert_class_and_value(rs, String, 1, "x") #Blob
    assert_class_and_value(rs, String, 2, " 1.10")
    assert_class_and_value(rs, String, 3, 'Bounded String Test                                                                                                                                                                                                                                            ')
    assert_class_and_value(rs, String, 4, 'Unbounded String Test')
    assert_class_and_value(rs, Fixnum, 5, 1)
    assert_class_and_value(rs, Fixnum, 6, 1)
    assert_class_and_value(rs, Fixnum, 7, 1) #Logical
    assert_date_and_time(rs, Date, 8, Date.new(1999, 1, 2))
    assert_date_and_time(rs, DateTime, 9, DateTime.new(1999, 1, 2, 21, 20, 53))
    assert_class_and_float_value(rs, Float, 10, 3.402823e+38, 1e+32 )

    assert_succeeded @api.ads_fetch_next(rs)
    assert_class_and_value(rs, String, 1, 255.chr) #Blob
    assert_class_and_value(rs, String, 2, "-1.1")
    assert_class_and_value(rs, String, 3, '                                                                                                                                                                                                                                                               ')
    assert_class_and_value(rs, NilClass, 4, nil)
    assert_class_and_value(rs, Fixnum, 5, 0)
    assert_class_and_value(rs, Fixnum, 6, 0)
    assert_class_and_value(rs, Fixnum, 7, 0) #Logical
    assert_class_and_value(rs, NilClass, 8, nil)
    assert_class_and_value(rs, NilClass, 9, nil)
    assert_class_and_float_value(rs, Float, 10, -3.402823e+38, 1e+32 )
    assert_nil @api.ads_free_stmt(rs)
  end

  def test_prepared_stmt
    stmt = @api.ads_prepare(@conn, "SELECT * FROM \"types\" WHERE \"id\" = ?")
    assert_not_nil stmt
    assert_equal 1, @api.ads_num_params(stmt)
    res, param = @api.ads_describe_bind_param(stmt, 0)
    assert_not_equal 0, res
    assert_equal Direction::DD_INPUT, param.get_direction()
    #assert_failed @api.ads_execute(stmt)

    assert_nil param.set_value(0);
    @api.ads_bind_param(stmt, 0, param)
    assert_succeeded @api.ads_execute(stmt)
    assert_succeeded @api.ads_fetch_next(stmt)
    assert_class_and_value(stmt, String, 3, "Bounded String Test                                                                                                                                                                                                                                            ")

    @api.ads_reset(stmt)
    assert_nil param.set_value(1);
    @api.ads_bind_param(stmt, 0, param)
    assert_succeeded @api.ads_execute(stmt)
    assert_succeeded @api.ads_fetch_next(stmt)
    assert_class_and_value(stmt, String, 3, "                                                                                                                                                                                                                                                               ")

    assert_nil @api.ads_free_stmt(stmt)
  end

  def test_insert_numeric
    assert_insert("_numeric_", " 1.10", String)
  end

  def test_insert_char
    assert_insert("_char_", "Bounded String Test                                                                                                                                                                                                                                            ", String)
  end

  def test_insert_memo
    assert_insert("_memo_", "Unbounded String Test", String)
  end

  def test_insert_int32
    assert_insert("_int_", 2147483646, Bignum)
    assert_insert("_int_", -2147483646, Bignum)
  end

  def test_insert_int8
    assert_insert("_short_", 32767, Fixnum)
    assert_insert("_short_", -32767, Fixnum)
  end

  def test_insert_date
    assert_insert("_date_", Date.new(1999, 1, 2), Date)
  end

  def test_insert_timestamp
    assert_insert("_timestamp_", DateTime.new(1999, 1, 2, 21, 20, 53), DateTime)
  end

  def test_insert_double
    assert_insert("_double_", 1.797, Float, 1e+38)
  end

  def assert_insert(column_name, value, type, delta = nil)
    @api.AdsBeginTransaction(@conn)
    stmt = @api.ads_prepare(@conn, 'INSERT INTO "types"("id", "' + column_name + '", "_logical_") VALUES(3, ?, true)')
    assert_not_nil stmt
    res, param = @api.ads_describe_bind_param(stmt, 0)
    if type == Date or type == DateTime then
      assert_nil param.set_value(value.strftime("%F %T"));
    else
      assert_nil param.set_value(value);
    end
    @api.ads_bind_param(stmt, 0, param)
    assert_succeeded @api.ads_execute(stmt)
    assert_nil @api.ads_free_stmt(stmt)

    rs = exec_direct_with_test('SELECT "' + column_name + '" FROM "types" WHERE "id" = 3')
    assert_succeeded @api.ads_fetch_next(rs)
    if type == Date or type == DateTime then
      assert_date_and_time(rs, type, 0, value)
    elsif type == Float
      assert_class_and_float_value(rs, type, 0, value, delta)
    else
      assert_class_and_value(rs, type, 0, value)
    end

    assert_nil @api.ads_free_stmt(rs)

    @api.ads_rollback(@conn)
  end

  def assert_column_info(rs, pos, expected_col_name, expected_col_type, expected_col_size)
    res, col_num, col_name, col_type, col_native_type, col_precision, col_scale, col_size, col_nullable = @api.ads_get_column_info(rs, pos);
    assert_succeeded res
    assert_equal expected_col_name, col_name
    assert_equal expected_col_type, col_type
    assert_equal expected_col_size, col_size
  end

  def assert_class_and_float_value(rs, cl, pos, expected_value, allowed_delta)
    res, val = @api.ads_get_column(rs, pos)
    assert_succeeded res
    assert_not_nil val unless expected_value.nil?
    assert_in_delta expected_value, val, allowed_delta
    assert_instance_of cl, val
  end

  def assert_date_and_time(rs, cl, pos, expected_value)
    res, val = @api.ads_get_column(rs, pos)
    assert_succeeded res
    assert_not_nil val unless expected_value.nil?
    parsed = cl.parse(val)
    assert_equal expected_value, parsed
    assert_instance_of cl, parsed
  end

  def assert_class_and_value(rs, cl, pos, expected_value)
    res, val = @api.ads_get_column(rs, pos)
    assert_succeeded res
    assert_not_nil val unless expected_value.nil?
    assert_equal expected_value, val
    assert_instance_of cl, val
  end

  def setup_transaction
    sql = "INSERT INTO test VALUES( DEFAULT );"
    assert_succeeded @api.ads_execute_immediate(@conn, sql)

    rs = exec_direct_with_test("SELECT LASTAUTOINC( CONNECTION ) FROM SYSTEM.IOTA")
    assert_succeeded @api.ads_fetch_next(rs)
    res, id = @api.ads_get_column(rs, 0)
    assert_succeeded res
    assert_not_nil id

    sql = "SELECT * FROM test where \"id\" = "  + id.to_s + ";"
    rs = @api.ads_execute_direct(@conn, sql)
    assert_not_nil rs

    assert_succeeded @api.ads_fetch_next(rs)
    assert_failed @api.ads_fetch_next(rs)
    assert_nil @api.ads_free_stmt(rs)
    id
  end

  def exec_direct_with_test(sql)
    rs = @api.ads_execute_direct(@conn, sql)
    code, msg =  @api.ads_error(@conn)
    assert_not_nil rs,  "SQL Code: #{code}; Message: #{msg}"
    rs
  end

  def assert_succeeded(val)
    assert_not_equal 0, val, @api.ads_error(@conn)
  end

  def assert_failed(val)
    assert_equal 0, val, @api.ads_error(@conn)
  end

end

