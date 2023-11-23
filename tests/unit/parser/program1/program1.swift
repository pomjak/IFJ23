// Program 1: Vypocet faktorialu (iterativne)
/* Hlavni telo programu */
write("Zadejte cislo pro vypocet faktorialu\n")
let a : Int? = readInt()
if let a {
	if (1==1) {write("Faktorial nelze spocitat\n")
	} else {
		var a = Int2Double(a)
		var vysl : Double = 1
		while (a > 0) {
			vysl = 1
			a = 1
		}
		write("Vysledek je: ", vysl, "\n")
	}
} else {
	write("Chyba pri nacitani celeho cisla!\n")
}
