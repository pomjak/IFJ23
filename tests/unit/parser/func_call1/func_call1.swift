/*
 * V případě, že příkaz volání funkce obsahuje jiný počet nebo typy skutečných parametrů, 
 * než funkce očekává (tedy než je uvedeno v její hlavičce, a to i u vestavěných funkcí, včetně potenciálního předání 
 * nil, kde to není očekáváno), jedná se o chybu 4.
*/

func a (_ x : String) -> Int
{
    return 1
}

a("Hello", 2) //Error 4