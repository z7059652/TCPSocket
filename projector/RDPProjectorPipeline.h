#pragma once
class RDPProjectorPipeline
{
public:
	RDPProjectorPipeline();
	~RDPProjectorPipeline();
	void* QuerydrdynvcPlugin(){ return m_rdynvcPlugin; }
	void* QueryrdpdrPlugin(){ return m_rdpdrPlugin; }
	void* QueryrdpsndPlugin(){ return m_rdpsndPlugin; };
	void Start();
	void Stop();
private:
	void* m_rdynvcPlugin;
	void* m_rdpdrPlugin;
	void* m_rdpsndPlugin;
};
