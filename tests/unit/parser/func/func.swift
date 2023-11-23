/*
 * Priklad ze zadani
 * V těle funkce jsou její parametry chápány jako předdefinované lokální proměnné s implicitní hodnotou 
*  danou skutečnými parametry, které nelze v těle funkce měnit. 
*  Výsledek funkce je dán provedeným příkazem návratu z funkce (viz sekce 4.4).
 */

func concat(_ x : String, with y : String) -> String 
{ 
    let x = x + y 
    return x + " " + y 
} 

let a = "ahoj " 

var ct : String 

ct = concat(a, with: "svete") 

write(ct, a)
// Error 0