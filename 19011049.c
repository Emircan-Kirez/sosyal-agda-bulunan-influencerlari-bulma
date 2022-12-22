/* Emircan Kirez - Ch3rry */
/* Son Güncelleme: 22/12/2022 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 255

// kisilerin bilgilerini tuttugum struct yapim
struct person{
	int id;
	char fname[25];
	char lname[25];
};

// adjList'imdeki her bir node icin struct yapim
struct node{
	int id;
	struct node *next;
};

// circular queue icin struct yapim
typedef struct queue{
	int qArray[MAX];
	int firstIndex, rearIndex;
}QUEUE;

//Function Prototypes (16)
int numOfLinesAFileHas();
void readFile(struct person *people, struct node **adjList);
struct node *createNode(int id);
void addNode(struct node **head, int id);
int *createInDegreeArray(struct node **adjList, int size);
int *eleminate(int *inDegree, int size, int M, struct node **adjList);
QUEUE *createQueue();
int isEmpty(QUEUE *q);
void enqueue(QUEUE *q, int value);
int dequeue(QUEUE *q);
int dfs(struct node **adjList, int *visited, int start, int end);
int *countConnection(struct node **adjList, int *validity, int size);
void showInDegrees(int *inDegree, int size);
void showValidPeople(struct person *people, int *validity, int *inDegree, int size);
void showInfluencers(struct person *people, int *inDegree, int *connection, int size, int X, int Y);
void showInfluencersDetailedMode(struct person *people, int *inDegree, int *connection, int size, int X, int Y);

int main(){
	struct person *people; // kisilerin bilgilerini tutan dizi
	struct node **adjList; // adjListim
	int *inDegree; // in-degree degerlerini tuttugum dizi
	int *validity; // eleme sonrasinda valid kalanlari tuttugum dizi
	int *connection; // validler icin hesaplanan ve kendisine kac dugumden yol oldugunu tutan dizi
	int i, M, X, Y; 
	int size = numOfLinesAFileHas() / 2; // satir sayisinin yarisi kadar kisi var
	
	people = (struct person *)malloc(sizeof(struct person) * size);
	
	adjList = (struct node **)malloc(sizeof(struct node *) * size);
	for(i = 0; i < size; i++){
		adjList[i] = NULL;
	}
	
	readFile(people, adjList); // dosyadan kisileri oku ve adjlist'i olustur
	inDegree = createInDegreeArray(adjList, size); // in-degree degerlerini hesapla
	
	printf("M, X, Y degerlerini yaziniz: ");
	scanf("%d %d %d", &M, &X, &Y);
	
	int mode; 
	printf("Normal mod icin 1'e, detay mod icin 2'e basiniz: ");
	do{
		scanf("%d", &mode);
		if(mode == 1){
			validity = eleminate(inDegree, size, M, adjList); // M degerinin altinda kalan kiþileri ele ve ona gore bir validity dizisi dondur
			connection = countConnection(adjList, validity, size); // validler icin connection sayisi hesapla ve dondur
			showInfluencers(people, inDegree, connection, size, X, Y); // X ve Y'ye gore influencer olan kisileri goster
		}else if(mode == 2){
			showInDegrees(inDegree, size); // in-degree dizisini goster
			validity = eleminate(inDegree, size, M, adjList); // M degerinin altinda kalan kiþileri ele ve ona gore bir validity dizisi dondur
			connection = countConnection(adjList, validity, size); // validler icin connection sayisi hesapla ve dondur
			showValidPeople(people, validity, inDegree, size); // elenmeyen kisileri goster
			showInfluencersDetailedMode(people, inDegree, connection, size, X, Y); // influencer olan kisilerin detayli bilgilerini goster
		}else{
			printf("Hatali giris yaptiniz. Tekrar deneyin...");
		}
	}while(mode != 1 && mode != 2);
	
	
	// deallocations
	free(people);
	
	for(i = 0; i < size; i++){
		free(adjList[i]);
	}
	free(adjList);
	
	free(inDegree);
	free(validity);
	free(connection);
	
	return 0;
}

// dosyada kac satir oldugunu bulur
int numOfLinesAFileHas(){ 
	FILE *fPtr = fopen("socialNET.txt", "r");
	int lineCount = 0;
	char buffer[MAX];
	
	while(fgets(buffer, MAX, fPtr) != NULL){
		lineCount++;
	}
	
	fclose(fPtr);
	return lineCount;
}

// dosyadan bilgileri okuyup people dizisine kaydeder ve adjListi olusturur
void readFile(struct person *people, struct node **adjList){
	FILE *fPtr = fopen("socialNET.txt", "r");
	char line[MAX], *word;
	int i;
	
	while(fgets(line, MAX, fPtr) != NULL){
		word = strtok(line, ",\r");  // dugum no'yu ayirma
		people[i].id = atoi(word);
		
		word = strtok(NULL, ",\r");  // ismi ayirma
		strcpy(people[i].fname, word);
		
		word = strtok(NULL, ",\r\n"); // soyismi ayirma
		strcpy(people[i].lname, word);
		
		fgets(line, MAX, fPtr); // yeni satir okuyup takip ettigi kisileri bulma
		word = strtok(line, ",\r");
		
		while(word != NULL){
			addNode(&adjList[i], atoi(word)); //takip ettigi nodelara gore adjList'ini doldurur
			word = strtok(NULL, ",\r\n");
		}
		i++;
	}
	
	fclose(fPtr);
}

// adjList'teki linkli listeler icin node olusturur
struct node *createNode(int id){
	struct node *newNode = (struct node *)malloc(sizeof(struct node));
	newNode->id = id;
	newNode->next = NULL;
	return newNode;
}

// adjList'teki istenen linkli listeye bastan node eklemesi yapar - karmasikligini azaltmak amaciyla basa eklenmistir
void addNode(struct node **head, int id){
	struct node *newNode = createNode(id);
	struct node *tmp = *head;
	*head = newNode;
	newNode->next = tmp;
}

// in-degree dizisini olusturur
int *createInDegreeArray(struct node **adjList, int size){
	int *array = (int *)calloc(size, sizeof(int));
	struct node *tmp;
	int i;

	for(i = 0; i < size; i++){ // her bir node icin
		tmp = adjList[i];
		while(tmp != NULL){ // her bir takip ettigi node icin
			array[tmp->id - 1]++; // dizi indisi 0 dan basladigi icin 1 azaltiyorum cunku 1. kisi 0. indiste
			tmp = tmp->next;
		}
	}
	
	return array;
}

// verilen M degerine gore valid olan yani elenmeyen node'lari bulur 
// topological sort with queue yonteminden esinlendim
int *eleminate(int *inDegree, int size, int M, struct node **adjList){
	int *validity = (int *)malloc(sizeof(int) * size);
	int i, value, indeks;
	QUEUE *q = createQueue();
	
	for(i = 0; i < size; i++){ // baslangicta herkes valid
		validity[i] = 1;
	}
	
	for(i = 0 ; i < size; i++){ // baslangicta indegree deðerleri M'nin altinda olanlari queue'ya at
		if(inDegree[i] < M){
			enqueue(q, i);
			validity[i] = 0; // queue'ya atilanlari invalid yapariz
		}
	}
	
	struct node *tmp;
	while(!isEmpty(q)){
		value = dequeue(q);   // queue'dan en bastaki degeri al
		tmp = adjList[value]; // onun adjList head'ini al ve sonuna kadar gez
		while(tmp != NULL){
			indeks = tmp->id - 1; // 1. kisi 0. indiste oldugu icin 1 azaltiyorum
			if(validity[indeks] && --inDegree[indeks] < M){ // komsularini bir azalt, M'in altina dustuyse onu da invalid yap ve queue'ya ekle
				enqueue(q, indeks);
				validity[indeks] = 0;
			}
			tmp = tmp->next;
		}
	}
	
	free(q);
	return validity;
}

/*--- QUEUE fonksiyonlari ---*/
/*
 • Queue olarak "Circular Queue" mantigi kullanilmistir.
 • Queue'nin boyutu yuksek verilmistir ve bu yuzden isFull kontrolu yapilmamistir.
 • Queue bos olmadigi surece dequeue yapmadigimizdan dequeue icinde isEmpty kontrolu yapilmamistir.
*/

// queue olusturur ve baslangic degerlerini set eder
QUEUE *createQueue(){
	QUEUE *tmp = (QUEUE *)malloc(sizeof(QUEUE));
	tmp->firstIndex = tmp->rearIndex = -1; //baslingicta bos yeri gostersin
	return tmp;
}

// queue bos mu?
int isEmpty(QUEUE *q){
	return q->firstIndex == -1 ? 1 : 0;
}

// enqueue islemi
void enqueue(QUEUE *q, int value){
	if(q->firstIndex == -1){ // ilk eleman ise
		q->firstIndex = 0;
	}
		
	q->rearIndex = (q->rearIndex + 1) % MAX; 
	q->qArray[q->rearIndex] = value;
}

// dequeue islemi
int dequeue(QUEUE *q){
	int value = q->qArray[q->firstIndex];
	if(q->firstIndex == q->rearIndex){ // eger dequeue yaptiktan sonra queue bos olacaksa baslangic durumuna donulmeli
		q->firstIndex = q->rearIndex = -1;
	}else{
		q->firstIndex = (q->firstIndex + 1) % MAX;
	}
	return value;
}

// start'ten end'e yol olup olmadigini dfs ile soyler : 1 -> var, 0 -> yok
int dfs(struct node **adjList, int *visited, int start, int end){ 
	int x;
    if (start == end)
        return 1;
        
    visited[start] = 1; // visited olarak isaretle
    struct node *tmp = adjList[start]; // komsularini tutan linkli listenin head'i 
    while (tmp != NULL) {
    	x = tmp->id - 1; // 1. kisi 0. indiste oldugu icin 1 azaltiyorum. start'tan x'e yol var, x'ten end'e yol var mi diye devam ediyorum
        if (!visited[x]){
        	if (dfs(adjList, visited, x, end)){ // dfs'ten 1 gelirse demekki yol var
        		return 1;
			}
		}
    	tmp = tmp->next;
    }
    return 0;
}

// valid node'lara kac farkli node'tan ulasabilecegini sayisini hesaplar ve dondurur
int *countConnection(struct node **adjList, int *validity, int size){
	int *connection = (int *)calloc(size, sizeof(int));
	int i, j;
	
	for(i = 0; i < size; i++){ 
		if(validity[i]){ // butun valid node'lar icin
			for(j = 0; j < size; j++){ // butun node'lardan ilgili node'a kac baglanti var
				int *tempVisited = (int *)calloc(size, sizeof(int));
				connection[i] += dfs(adjList, tempVisited, j, i); // baglanti varsa 1, yoksa 0 donecegi icin if ile kontrol yerine topluyorum
				free(tempVisited);
			}
			connection[i]--; // kendini de saydigi icin siliyorum -- if kontrolu -> if(i != j) eklemek yerine boyle yaptim 
		}
	}
	return connection;
}

// In-degree degerlerini gosterir
void showInDegrees(int *inDegree, int size){
	printf("\n----- In-Degree Degerleri -----\n");
	int i;
	for(i = 0; i < size; i++){
		printf("Dugum No: %2d | In-degree: %d\n", i+1, inDegree[i]);
	}
}

// elenmeyen kisileri gosterir
void showValidPeople(struct person *people, int *validity, int *inDegree, int size){
	printf("\n----- Elenmeyen Kisilerin Bilgileri -----\n");
	int i;
	for(i = 0; i < size; i++){
		if(validity[i]){
			printf("Dugum No: %2d | Ad Soyad: %15s %15s | In-degree: %d\n", i+1, people[i].fname, people[i].lname, inDegree[i]);
		}
	}
}

// normal modda influencer bilgilerini gosterir
void showInfluencers(struct person *people, int *inDegree, int *connection, int size, int X, int Y){
	printf("\n----- Influencer Bilgileri -----\n");
	int i;
	for(i = 0; i < size; i++){
		if(inDegree[i] >= X && connection[i] >= Y){ // valid olmayanlarin connection sayisi hesaplanmadigindan ve Y'ye 0 veremeyecegimizden dolayi bu kontrol yeterli
			printf("Dugum No: %2d | Ad Soyad: %s %s\n", i+1, people[i].fname, people[i].lname);
		}
	}
}

// detay modda influencer bilgilerini gosterir
void showInfluencersDetailedMode(struct person *people, int *inDegree, int *connection, int size, int X, int Y){
	printf("\n----- Influencer Bilgileri -----\n");
	int i;
	for(i = 0; i < size; i++){
		if(inDegree[i] >= X && connection[i] >= Y){ // valid olmayanlarin connection sayisi hesaplanmadigindan ve Y'ye 0 veremeyecegimizden dolayi bu kontrol yeterli
			printf("Dugum No: %2d | Ad Soyad: %15s %15s | In-degree: %2d | Baglanti Sayisi: %d\n", i+1, people[i].fname, people[i].lname, inDegree[i], connection[i]);
		}
	}
}
