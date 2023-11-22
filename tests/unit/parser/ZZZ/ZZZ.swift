//Test variable declarations with different data types

var integerVariable: Int = 10
var doubleVariable: Double = 3.14
var stringVariable: String = "Hello, World!"
var integerVariable_nil: Int? = 10
var doubleVariable_nil: Double? = 3.14
var stringVariable_nil: String? = "Hello, World!"

// Test variable initialization with different values

var implicitIntegerVariable = 20
var implicitDoubleVariable = 1.618
var implicitStringVariable = "Welcome"


// Test const initialization with different values

let integerConst: Int = 30
let doubleConst: Double = 6.28
let stringConst: String = "Oznuk seems okey"
let integerConst_nil: Int? = 10
let doubleConst_nil: Double? = 3.14
let stringConst_nil: String? = "Hello, World!"

let implicitIntegerConst = 20
let implicitDoubleConst = 1.618
let implicitStringConst = "Welcome"

function_nil_nil()
function_Int_nil(implicitIntegerVariable)
function_Double_nil(implicitDoubleVariable)
function_String_nil(implicitStringVariable)

var ret_int =function_nil_Int()
ret_int = function_Int_Int(implicitIntegerVariable)
ret_int = function_Double_Int(implicitDoubleVariable)
ret_int = function_String_Int(implicitStringVariable)

var ret_dbl = function_nil_Double()
ret_dbl =  function_Int_Double(implicitIntegerVariable)
ret_dbl = function_Double_Double(implicitDoubleVariable)
ret_dbl = function_String_Double(implicitStringVariable)

var ret_string = function_nil_String()
ret_string = function_Int_String(implicitIntegerVariable)
ret_string = function__Double_String(implicitDoubleVariable)
ret_string = function_String_String(implicitStringVariable)


func function_nil_nil ()
{
	return
}

func function_Int_nil (_ x : Int)
{
	return 
}

func function_Double_nil (_ x : Double)
{
	return
}

func function_String_nil (_ x : String)
{
	return
}


func function_nil_Int () -> Int
{
	return 1
}

func function_Int_Int (_ x : Int) -> Int
{
	return 1
}

func function_Double_Int (_ x : Double) -> Int
{
	return 1
}

func function_String_Int (_ x : String) -> Int
{
	return 1
}



func function_nil_Double () -> Double
{
	return 1.0
}

func function_Int_Double (_ x : Int) -> Double
{
	return 1.0
}

func function_Double_Double (_ x : Double) -> Double
{
	return 1.0
}

func function_String_Double (_ x : String) -> Double
{
	return 1.0
}



func function_nil_String () -> String
{
	return "string"
}

func function_Int_String (_ x : Int) -> String
{
	return "string"
}

func function__Double_String (_ x : Double) -> String
{
	return "string"
}

func function_String_String (_ x : String) -> String
{
	return "string"
}