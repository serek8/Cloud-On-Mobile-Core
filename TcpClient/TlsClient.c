//
//  TlsClient.c
//  CloudOnMobile
//
//  Created by Cloud On Mobile Team on 01/12/2021.
//

#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include "TlsClient.h"

SSL *ssl;

int create_socket(char url_str[], BIO *out) ;

void init_openssl_library(void)
{
  (void)SSL_library_init();

  SSL_load_error_strings();

  /* ERR_load_crypto_strings(); */
  
  OPENSSL_config(NULL);
    
  /* Include <openssl/opensslconf.h> to get this define */
#if defined (OPENSSL_THREADS)
  fprintf(stdout, "Warning: thread locking is not implemented\n");
#endif
}

void http_client_init(void)
{
    static int done = 0;
    if(done == 1) return;
    done = 1;
    SSL_library_init();
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
}

void CORE_HttpClient_loadDefaultCerts(SSL_CTX *ctx);

int tlsHanshake(int fd){
  http_client_init();
  /*
   * Configure SSL context
   */
  SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
  
  /*
   * Set up SSL certs and policies
   */
  
  /*
   * Configure the connection
   */
  
//  BIO * bio = BIO_new_ssl_connect(ctx);
//  BIO_set_conn_hostname(bio, hostname_port_c);
  ssl = SSL_new(ctx);
//  int fd_from_tcp_client = server;
  SSL_set_fd(ssl, fd);
  
  int connect_result = SSL_connect(ssl);
  int err = SSL_get_error(ssl, connect_result);
  printf("connect_result = %s", SSL_error_description(err));
  if ( connect_result != 1 )
    printf("Error: Could not build a SSL session to: %s.\n", "MY_DOMAIN");
  else
    printf("Successfully enabled SSL/TLS session to: %s.\n", "MY_DOMAIN");
  
  X509                *cert = NULL;
  X509_NAME       *certname = NULL;
  
  cert = SSL_get_peer_certificate(ssl);
  if (cert == NULL) printf("");
//      BIO_printf(outbio, "Error: Could not get a certificate from: %s.\n", dest_url);
  else printf("");
//      BIO_printf(outbio, "Retrieved the server's certificate from: %s.\n", dest_url);

    /* ---------------------------------------------------------- *
     * extract various certificate information                    *
     * -----------------------------------------------------------*/
    certname = X509_NAME_new();
    certname = X509_get_subject_name(cert);

  
  return 0;
}

void CORE_HttpClient_loadCert(char pem[], X509_STORE *cert_store) {
    BIO * bio;
    bio = BIO_new_mem_buf(pem, (int)strlen(pem));
    X509 *pinned_cert = PEM_read_bio_X509(bio, NULL, NULL, NULL);
    X509_STORE_add_cert(cert_store, pinned_cert);
    printf("Imported Certificate: %s\n",X509_NAME_oneline(X509_get_subject_name(pinned_cert), NULL, 0));
}

char CORE_cert_app_root[];
void CORE_HttpClient_loadDefaultCerts(SSL_CTX *ctx){
    X509_STORE * cert_store = X509_STORE_new();
    CORE_HttpClient_loadCert(CORE_cert_app_root, cert_store);
    // load_cert(inter_ca_cert1, cert_store); // just load more to add more
    SSL_CTX_set_cert_store(ctx, cert_store);
}


char CORE_cert_app_root[] =
"-----BEGIN CERTIFICATE-----\n"
"MIIG7zCCBNegAwIBAgIUDC0ZVcyuxJW2sgrSYKD1VsoU2d4wDQYJKoZIhvcNAQEN\n"
"BQAwgaMxCzAJBgNVBAYTAlBsMQ8wDQYDVQQIDAZMdWJsaW4xDzANBgNVBAcMBkx1\n"
"YmxpbjESMBAGA1UECgwJQXBwQnVua2VyMRowGAYDVQQLDBFzZXJlZHluc2tpLmNv\n"
"bS1jYTEaMBgGA1UEAwwRQXBwQnVua2VyLUNBLVJvb3QxJjAkBgkqhkiG9w0BCQEW\n"
"F2phbnNlcmVkeW5za2lAZ21haWwuY29tMB4XDTIwMDYwNjEwMTcwMFoXDTIxMDYw\n"
"NTEwMTcwMFowgaMxCzAJBgNVBAYTAlBsMQ8wDQYDVQQIDAZMdWJsaW4xDzANBgNV\n"
"BAcMBkx1YmxpbjESMBAGA1UECgwJQXBwQnVua2VyMRowGAYDVQQLDBFzZXJlZHlu\n"
"c2tpLmNvbS1jYTEaMBgGA1UEAwwRQXBwQnVua2VyLUNBLVJvb3QxJjAkBgkqhkiG\n"
"9w0BCQEWF2phbnNlcmVkeW5za2lAZ21haWwuY29tMIICIjANBgkqhkiG9w0BAQEF\n"
"AAOCAg8AMIICCgKCAgEA3LsWP19OvLI3s9wS603n6wpsWF7oTyM652ES3MBOh+Ph\n"
"tSlpxwaOlJv2pk2kP1K7xa6w+CcisXg+EEdXsxVnhkW4KhNmr4NfTMGHRKZdccY9\n"
"hRa2ZZk/7i6g7kxlkny4nZ3W++V0omu0lkDOgHwx66/oxky+S5yU1y0s/g7MfPzp\n"
"qzKlSEs2JA0uOd34yabxKr1Lx3joPSO23KfAWWx7Z76akvEhkSbTRZ7Rw8vH6kCZ\n"
"PfLQpbacJli60Wty71QK4NJj/tkF3SL+/GyqISIZ1plm2oqj55iNzz7Mio8khLQE\n"
"QyQXLoZ8aCVR8hynfkupeqZUbTc2U9mdSwhQpRlLMUTkcpPmmDwGcCBSZ428n+3x\n"
"9twLRzgmLQtdNAVl+xSfcA9OuBxOIyjy7FIjo4SI/IWdwIQTW39xb+oT2vlrUytG\n"
"cV9IdHn+a4M6tpXhBZQs4sbuYs6EVT1DB6JC8Ifh9T9HK+yzDs3bEnRKYoN0oDXD\n"
"bdRz3uYmXI8AMVAEW4iXOh9wBUxlsm+DNGgqlX1WvY+v2Plfte7Qr4RsDsKTSbcp\n"
"A44kFE4q3lnAyTHyikIYUhXobsKb1ZoUm6uVNieOw74+D6rOcpRx7rThxGrdSFaf\n"
"UUj1AciLxNk01qPzPY9GruQ2oEMtvUlhzfrYKSRmGEyCEIcH0oAWxQ8PhKtEG/EC\n"
"AwEAAaOCARcwggETMB0GA1UdDgQWBBT2pEbUAjBDS+b1Kdj0Yew5jS4o9zCB4wYD\n"
"VR0jBIHbMIHYgBT2pEbUAjBDS+b1Kdj0Yew5jS4o96GBqaSBpjCBozELMAkGA1UE\n"
"BhMCUGwxDzANBgNVBAgMBkx1YmxpbjEPMA0GA1UEBwwGTHVibGluMRIwEAYDVQQK\n"
"DAlBcHBCdW5rZXIxGjAYBgNVBAsMEXNlcmVkeW5za2kuY29tLWNhMRowGAYDVQQD\n"
"DBFBcHBCdW5rZXItQ0EtUm9vdDEmMCQGCSqGSIb3DQEJARYXamFuc2VyZWR5bnNr\n"
"aUBnbWFpbC5jb22CFAwtGVXMrsSVtrIK0mCg9VbKFNneMAwGA1UdEwQFMAMBAf8w\n"
"DQYJKoZIhvcNAQENBQADggIBAAv5OIJKqW2O6VGgyIYKqXcWaFLlOovZBPwyScZo\n"
"npR+VDpdjujQpDZ9mh/1WXldFKbQH3H60K0dZlNYeVJndjW7NgiotN903Xunpp+D\n"
"fwkTi+QvZ0C9WjajGnV4HD6OG2vJaMJRa1xKHc7xFGtAIGGRWHNRuXxY4LwshM1T\n"
"kUdFI+LFYck0KZqMvrjuSldCvvT4+J34aBTxSs2kvgoP0jqMtyhL862uMoFHli/Y\n"
"KYG1PrmpNprw+oQ9anNfHnaiwAiNJen+wGgnKTJFpDaVMIwuWEWww3MIL5y2KnNa\n"
"3/eRReNo+qjrEwVh1dXTZ7rDqeqPQ+X7JUpGnvoWrtgNymoIq/0CPvn0u3SHiMHI\n"
"fryIs4HQINgv8RZ6xyN/f7fVDvyO9k1APEXNMGjzfP3uC5ECvEd6D+saI9KvLHq+\n"
"bhCWv/zShFtVCHaDLKc3gX3t8KyTnyeryo3UgSPBs/1jQm9HNUuDcbUi2WOg48vC\n"
"rxhrV7ebsRkfG13l427jXgqB4plmIjyWiTHrRMGvireqPfoT+qu9IxXTdt5QlO/X\n"
"E6nMkcLatZdIVnPP2mGCcPSHd8M+AP+9msqzjSKyHwPs+d1luZAs4W4xTSUHSncn\n"
"BJyA2BHrmcI7qVDcB1F3UZK3GzLrSp27cvY4nM3EtVZtsJ/vzFWV75nEKMq4Qbhl\n"
"SN1k\n"
"-----END CERTIFICATE-----";


int create_socket(char url_str[], BIO *out) {
  int sockfd;
  char hostname[256] = "";
  char    portnum[6] = "443";
  char      proto[6] = "";
  char      *tmp_ptr = NULL;
  int           port;
  struct hostent *host;
  struct sockaddr_in dest_addr;

  /* ---------------------------------------------------------- *
   * Remove the final / from url_str, if there is one           *
   * ---------------------------------------------------------- */
  if(url_str[strlen(url_str)] == '/')
    url_str[strlen(url_str)] = '\0';

  /* ---------------------------------------------------------- *
   * the first : ends the protocol string, i.e. http            *
   * ---------------------------------------------------------- */
  strncpy(proto, url_str, (strchr(url_str, ':')-url_str));

  /* ---------------------------------------------------------- *
   * the hostname starts after the "://" part                   *
   * ---------------------------------------------------------- */
  strncpy(hostname, strstr(url_str, "://")+3, sizeof(hostname));

  /* ---------------------------------------------------------- *
   * if the hostname contains a colon :, we got a port number   *
   * ---------------------------------------------------------- */
  if(strchr(hostname, ':')) {
    tmp_ptr = strchr(hostname, ':');
    /* the last : starts the port number, if avail, i.e. 8443 */
    strncpy(portnum, tmp_ptr+1,  sizeof(portnum));
    *tmp_ptr = '\0';
  }

  port = atoi(portnum);

  if ( (host = gethostbyname(hostname)) == NULL ) {
    BIO_printf(out, "Error: Cannot resolve hostname %s.\n",  hostname);
    abort();
  }

  /* ---------------------------------------------------------- *
   * create the basic TCP socket                                *
   * ---------------------------------------------------------- */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  dest_addr.sin_family=AF_INET;
  dest_addr.sin_port=htons(port);
  dest_addr.sin_addr.s_addr = (in_addr_t) *(long*)(host->h_addr);

  /* ---------------------------------------------------------- *
   * Zeroing the rest of the struct                             *
   * ---------------------------------------------------------- */
  memset(&(dest_addr.sin_zero), '\0', 8);

  tmp_ptr = inet_ntoa(dest_addr.sin_addr);

  /* ---------------------------------------------------------- *
   * Try to make the host connect here                          *
   * ---------------------------------------------------------- */
  if ( connect(sockfd, (struct sockaddr *) &dest_addr,
                              sizeof(struct sockaddr)) == -1 ) {
    BIO_printf(out, "Error: Cannot connect to host %s [%s] on port %d.\n",
             hostname, tmp_ptr, port);
  }

  return sockfd;
}
