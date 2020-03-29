# Necessary libraries
import requests
import json
import time
import re
import os

def main( ):
    with open( 'login_data.json' ) as login_data:
        login = json.load( login_data )
     
    login_data.close( )
        
    if not login:
        return
        
    if len( login[ 'Usuario' ] ) < 2:
        return
    
    s = requests.Session( )
    
    user_agent = 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_3) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.47 Safari/537.36'
    
    s.post( 'https://portal.p4ed.com/', data = login, headers = { 'User-Agent': user_agent } )
    
    resp = s.get( 'https://portal.p4ed.com/Login/AutenticacaoPortalEdrosAlunos/0' )
    
    login_token = re.search( r"'https:\/\/student\.p4ed\.com\/login\/loginbytoken\/(.+)?'", resp.text ).group( )
    login_token = login_token[ 1 : -1 ]

    s.get( login_token )
    
    with open( 'homework.json' ) as hw_file:
        hw_data = json.load( hw_file )
        
    hw_file.close( )
        
    if not hw_data:
        return;
    
    if not hw_data[ 'concluidas' ]:
        return
    
    resp = s.post( 'https://student.p4ed.com/tarefas/salvar', data = hw_data )
    resp_json = json.loads( resp.text )
    
    with open( 'result.json', 'w+', encoding = 'utf8' ) as result_file:
        json.dump( resp_json, result_file )
        
    result_file.close( )
    os.remove( 'homework.json' )
    
if __name__ == '__main__':
    main( )