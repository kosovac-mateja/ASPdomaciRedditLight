#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <algorithm>
#include <windows.h>

using namespace std;

//funkcija za "ciscenje ekrana"
// preuzeta sa interneta radi izbegavanja koriscenja funkcije system("CLS")
void clear_screen(void)
{
	DWORD n; //broj ispisanih karaktera
	DWORD size; //broj vidljivih karaktera
	COORD coord = { 0 }; //gornja leva pozicija ekrana
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleScreenBufferInfo(h, &csbi);

	//broj karaktera koje treba overvrajtovati
	size = csbi.dwSize.X * csbi.dwSize.Y;

	//Overvrajt sa spejsovima
	FillConsoleOutputCharacter(h, TEXT(' '), size, coord, &n);
	GetConsoleScreenBufferInfo(h, &csbi);
	FillConsoleOutputAttribute(h, csbi.wAttributes, size, coord, &n);

	//podesavanje kursora gore levo
	SetConsoleCursorPosition(h, coord);
}

//prekida program ukoliko je korisnik uneo nesto sto nije int na mesto inta
void intGreska()
{
	if (cin.fail())
	{
		clear_screen();

		cout << "Nepravilan unos!";

		Sleep(1000);

		exit(0);
	}
}

//prekida program
void nepostojeciIdGreska()
{
	clear_screen();

	cout << "Uneti id ne postoji!";

	Sleep(1000);

	exit(0);
}



class Cvor
{
public:
	string tekst; //tekst pitanja/odgovora

	static int br;

	int id; //id pitanja/odgovora

	vector<int> ocena; //niz ocena sinova

	int dubina; //dubina cvora u stablu(samo se kod ispisa koristi)

	vector<Cvor*> sinovi; //niz pokazivaca na sinove

	Cvor* otac; //pokazivac na oca


	Cvor(string str, int h, Cvor* tata) : tekst(str), dubina(h), id(br++), otac(tata) {}

	~Cvor()
	{
		int duzina = sinovi.size();

		for (int i = 0; i < duzina; i++)
			delete sinovi[i];
	}

	void dodajSina(string str) 
	{
		if (sinovi.size() < 10)
		{
			sinovi.push_back(new Cvor(str, dubina + 1, this));
			ocena.push_back(0);
		}

		else
			cout << "Ne sme biti vise od 10 odgovora!";
	}

	void printCvor() const { cout << string(3 * (dubina - 1), ' ') << "#" << id << " " << tekst << " "; }

	void printPodstablo()
	{
		stack<Cvor*> stek;

		stek.push(this);

		while (!stek.empty())
		{
			Cvor* tren = stek.top();
			
			stek.pop();

			tren->printCvor();

			cout << '(';
			for (int i = 0; i < tren->ocena.size(); i++) //ispis ocena sinova
				cout << tren->ocena[i] << " ";
			cout << ")\n";

			for (int i = tren->sinovi.size() - 1; i >= 0; i--) //stavljam sve sinove od kraja
				stek.push(tren->sinovi[i]);

		}
	}

	int maxOcena() //vraca maksimalnu ocenu sinova
	{
		int max = 0;

		for (int i = 0; i < ocena.size(); i++)
		{
			if (ocena[i] > max)
				max = ocena[i];
		}

		return max;
	}

	//zamena dva cvora
	inline void swapCvor(Cvor** c1, Cvor** c2)
	{
		Cvor* tren = *c1;

		*c1 = *c2;

		*c2 = tren;
	}

	//zamena dva inta
	inline void swapOcena(int& a, int& b)
	{
		int tren = a;

		a = b;

		b = tren;
	}

	//sortiranje sinova po nerastucem poretku
	void sortSinove() 
	{
		if (sinovi.size() > 1)
		{
			for (int i = 0; i < sinovi.size() - 1; i++)
			{
				if (ocena[i] < ocena[i + 1])
				{
					swapCvor(&sinovi[i], &sinovi[i + 1]);

					swapOcena(ocena[i], ocena[i + 1]);

					i = -1;
				}
			}
		}
	}

	//ako postoji sin, vraca njegov indeks
	int postoji(int id)
	{
		for (int i = 0; i < sinovi.size(); i++)
		{
			if (sinovi[i]->id == id)
				return i;
		}

		return -1;
	}
};



class Pitanje
{
public:
	Cvor* koren;

	Pitanje(string pitanje = "") : koren(new Cvor(pitanje, 1, nullptr)) {}

	void print() const { koren->printPodstablo(); }

	void sortPitanje() //obilazak svih cvorova po preorderu i sort njihovih sinova
	{
		stack<Cvor*> stek;

		stek.push(koren);

		while (!stek.empty())
		{
			Cvor* tren = stek.top();

			stek.pop();

			tren->sortSinove();

			for (int i = tren->sinovi.size() - 1; i >= 0; i--) //stavljam sve sinove od kraja
				stek.push(tren->sinovi[i]);
		}
	}
};

//pretraga pitanja po id-u
Pitanje* pretragaPitanja(vector<Pitanje>& niz, int id)
{
	for (int i = 0; i < niz.size(); i++)
	{
		if (niz[i].koren->id == id)
		{
			return &niz[i];
		}
	}
}

//pretraga odgovora po id-u(preorder)
Cvor* pretragaOdgovora(Pitanje &p, int id)
{
	stack<Cvor*> stek;

	stek.push(p.koren);

	while (!stek.empty())
	{
		Cvor* tren = stek.top();

		stek.pop();

		if (id == tren->id) { return tren; }

		for (int i = tren->sinovi.size() - 1; i >= 0; i--) 
			stek.push(tren->sinovi[i]);
	}
}

//dodaje odgovor nekom pitanju
void dodajOdgovor(Cvor** cvor, string odgovor) { (*cvor)->dodajSina(odgovor); } 

//dodavanje novog pitanja
void novoPitanje(vector<Pitanje>& niz)
{
	string str;

	cout << "Unesite svoje pitanje: ";

	cin.ignore();

	getline(cin, str);

	niz.push_back(Pitanje(str));
}

//ispisivanje svih pitanja
void ispisSvihPitanja(const vector<Pitanje>& niz)
{
	cout << "Pitanja:\n\n";
	for (int i = 0; i < niz.size(); i++)
	{
		niz[i].koren->printCvor();

		cout << '\n';
	}
}

//da li postoji pitanje u nizu pitanja
int postojiPitanje(const vector<Pitanje>& niz, int id)
{
	for (int i = 0; i < niz.size(); i++)
	{
		if (niz[i].koren->id == id)
			return i;
	}

	return -1;
}

//dodavanje odgovora na pitanje
void odgovorNaPitanje(vector<Pitanje>& niz)
{
	int id;

	string str;

	Pitanje* p;

	cout << "\n\nNa koje pitanje zelite da odgovorite?\n:";

	cin >> id;

	intGreska();

	if (postojiPitanje(niz, id) == -1)
		nepostojeciIdGreska();

	p = pretragaPitanja(niz, id);

	cout << "\nOdgovor: ";

	cin.ignore();

	getline(cin, str);

	dodajOdgovor(&(p->koren), str); 
}

//brisanje pitanja i svih njegovih odgovora
void brisanjePitanja(vector<Pitanje>& niz)
{
	int id;

	cout << "\n\nKoje pitanje zelite da obrisete?\n: ";

	cin >> id;

	intGreska();

	if (postojiPitanje(niz, id) == -1)
		nepostojeciIdGreska();

	for (int i = 0; i < niz.size(); i++)
	{
		if (niz[i].koren->id == id)
			niz.erase(niz.begin() + i);
	}
}

//ispisivanje pitanja i svih njegovih odgovora
Pitanje* ispisPitanjaiOdgovora(vector<Pitanje>& niz)
{
	int id;

	Pitanje* p;

	cout << "\n\nKoje pitanje zelite da ispisete?\n:";

	cin >> id;

	intGreska();

	if (postojiPitanje(niz, id) == -1)
		nepostojeciIdGreska();

	clear_screen();

	p = pretragaPitanja(niz, id);

	p->print();

	return p;
}

//dodavanje komentara na odgovor
void komentarNaOdgovor(Pitanje *p)
{
	int id;

	string str;

	cout << "\n\nKoji odgovor zelite da komentarisete? \n: ";

	cin >> id;

	intGreska();

	if (p->koren->postoji(id) == -1)
		nepostojeciIdGreska();

	Cvor* tren = pretragaOdgovora(*p, id);

	cout << "\nKomentar: ";

	cin.ignore();

	getline(cin, str);

	dodajOdgovor(&tren, str);
}

//brisanje odgovora
void brisanjeOdgovora(Pitanje *p)
{
	int id;

	cout << "\nKoji odgovor zelite da obrisete?\n: ";

	cin >> id;

	intGreska();

	if (p->koren->postoji(id) == -1)
		nepostojeciIdGreska();

	stack<Cvor*> stek;

	stek.push((*p).koren);

	while (!stek.empty())
	{
		Cvor* tren = stek.top();

		stek.pop();

		int i = tren->postoji(id);

		if (i != -1)
		{
			tren->sinovi.erase(tren->sinovi.begin() + i);

			tren->ocena.erase(tren->ocena.begin() + i);

			break;
		}

		for (int i = tren->sinovi.size() - 1; i >= 0; i--) //stavljam sve sinove od kraja
			stek.push(tren->sinovi[i]);
	}
}

//povecanje ocene nekom odgovoru
void povecanjeOcene(Pitanje* p)
{
	int id, up;

	cout << "\n\nKom odgovoru zelite da povecate ocenu?\n: ";

	cin >> id;

	intGreska();

	if (p->koren->postoji(id) == -1)
		nepostojeciIdGreska();

	cout << "\nZa koliko zelite da povecate ocenu?\n: ";

	cin >> up;

	intGreska();

	if (up > 0)
	{
		Cvor* tren = pretragaOdgovora(*p, id);

		Cvor* otac = tren->otac;

		otac->ocena[otac->postoji(id)] += up;
	}
}

//meni kada se izabere ispis pojedinacnog pitanja
void meni3(Pitanje* p)
{
	string str;

	int izbor, up;

	cout << "\n\n\n[1] Dodaj komentar na odgovor \n";

	cout << "[2] Brisi odgovor \n";

	cout << "[3] Povecanje ocene\n";

	cout << "[4] Povratak na pocetni meni\n\n\n: ";

	cin >> izbor;

	switch (izbor)
	{
	case 1:
		komentarNaOdgovor(p);
		break;
	case 2:
		brisanjeOdgovora(p);
		break;
	case 3:
		povecanjeOcene(p);
		break;
	case  4:
		break;
	default:
		intGreska();
		break;
	}
}

//meni kada se izabere ispis svih pitanja
void meni2(vector<Pitanje>& niz)
{
	int izbor;

	Pitanje* p;

	cout << "\n\n\n[1] Dodajte odgovor/komentar \n";
	cout << "[2] Ispis pojedinacnog pitanja i njegovih odgovora/komentara \n";
	cout << "[3] Brisanje pitanja i svih njegovih odgovora/komentara \n";
	cout << "[4] Povratak na glavni meni \n\n\n: ";

	cin >> izbor;

	switch (izbor)
	{
	case 1:
		odgovorNaPitanje(niz);
		break;

	case 2:
		p = ispisPitanjaiOdgovora(niz);

		meni3(p);
		break;
	case 3:
		brisanjePitanja(niz);
		break;
	case 4:
		break;
	default:
		intGreska();
		break;
	}
}

//najveca ocena unutar jednog pitanja(preorder)
int najvecaOcena(Pitanje &p)
{
	int max = 0;

	stack<Cvor*> stek;

	stek.push(p.koren);

	while (!stek.empty())
	{
		Cvor* tren = stek.top();

		stek.pop();

		if (max < tren->maxOcena())  
			max = tren->maxOcena(); 

		for (int i = tren->sinovi.size() - 1; i >= 0; i--) 
			stek.push(tren->sinovi[i]);
	}

	return max;
}

//najveca ocena medju svim pitanjima
Pitanje pitanjeNajvecaOcena(vector<Pitanje>& niz)
{
	int max = 0;

	Pitanje q = niz[0];

	for (int i = 0; i < niz.size(); i++)
	{
		if (najvecaOcena(niz[i]) > max)
		{
			max = najvecaOcena(niz[i]);
			q = niz[i];
		}
	}

	return q;
}

//sortira se pitanje po pitanje
void sortiranje(vector<Pitanje>& niz)
{
	for (int i = 0; i < niz.size(); i++)
		niz[i].sortPitanje();
}

//glavni meni
void meni(vector<Pitanje>& nizPitanja)
{
	int opcija;

	string str;

	Pitanje p;

	while (true)
	{
		clear_screen();
		cout << "[1] Postavite novo pitanje \n";
		cout << "[2] Ispis svih pitanja \n";
		cout << "[3] Uredjivanje odgovora prema njihovoj oceni \n";
		cout << "[4] Pronalazak pitanja ciji odgovor/komentar ima najvecu ocenu \n";
		cout << "[5] Prekid programa \n\n\n: ";

		cin >> opcija;

		clear_screen();

		switch (opcija)
		{
		case 1:
			novoPitanje(nizPitanja);
			break;
		case 2:
			if (nizPitanja.size() > 0)
			{
				ispisSvihPitanja(nizPitanja);

				meni2(nizPitanja);
			}

			else
			{
				cout << "Trenutno ne postoji nijedno pitanje!";

				Sleep(1000);
			}
			break;
		case 3:
			sortiranje(nizPitanja);
			break;
		case 4:
			if (nizPitanja.size() > 0)
			{
				p = pitanjeNajvecaOcena(nizPitanja);

				p.print();

				cout << "\n\n\n[1] Povratak na pocetni meni\n\n\n:";

				cin >> opcija;
			}

			else
			{
				cout << "Trenutno ne postoji nijedno pitanje!";

				Sleep(1000);
			}
			break;
		case 5:
			exit(0);
			break;
		default:
			intGreska();
			break;
		}
	}
}

int Cvor::br = 0;

int main()
{
	vector<Pitanje> nizPitanja;

	meni(nizPitanja);
}