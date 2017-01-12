#define HELO "HELO"
#define OKOK "OKOK"
#define BYEE "BYEE"
#define BCST "BCST"
#define PRVT "PRVT"
#define LIST "LIST"
#define SHUT "SHUT"
#define BADD "BADD"
#define DEBG "DEBG"

struct message *helo(int pseudo_size, char *pseudo_str, int tube_size, char *tube);
struct message *okok(int id_size, char *id_str);
struct message *badd();
struct message *byee(int id_size, char *id_str);
struct message *bcst_client(int id_size, char *id_str, int txt_size, char *txt_str);
struct message *bcst_server(int pseudo_size, char *pseudo_str, int txt_size, char *txt_str);
struct message *prvt_client(int id_size, char *id_str, int pseudo_size, char *pseudo_str, int txt_size, char *txt_str);
struct message *prvt_server(int pseudo_size, char *pseudo_str, int txt_size, char *txt_str);
struct message *list_client(int id_size, char *id_str);
struct message *list_server(int n_size, char *n_str, int pseudo_size, char *pseudo_str);
struct message *shut_client(int id_size, char *id_str);
struct message *shut_server(int pseudo_size, char *pseudo_str);
struct message *debg();
