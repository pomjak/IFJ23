/*
 * V případě, že příkaz volání funkce obsahuje jiný počet nebo typy skutečných parametrů, 
 * než funkce očekává (tedy než je uvedeno v její hlavičce, a to i u vestavěných funkcí, včetně potenciálního předání 
 * nil, kde to není očekáváno), jedná se o chybu 4.
*/

func aaa(_ a: Int, with b: Int) -> Int? 
{
    return 100
}
aaa(50, with: 60)
let bbb: Int 
aaa(50, with: bbb)
aaa()//Error 4