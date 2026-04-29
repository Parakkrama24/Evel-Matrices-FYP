void calculate_average_anonymity_degree()
{
    // This variable accumulates the total anonymity values (Σ df)
    double anonymity_sum = 0.0;

    // Counts how many flows require privacy (|Fψ|)
    uint32_t privacy_flow_counter = 0;

    // Loop through all flows in the system
    for (uint32_t fid = 0; fid < 2 * flows; fid++)
    {
        // ψf → privacy demand of flow f (assigned by LLM)
        double psi_f = flow_privacy_value[fid];

        // τψ → privacy threshold (only flows above this use privacy mode)
        double tau_psi = 0.5;

        // Check if this flow requires privacy (ψf > τψ)
        // This corresponds to selecting flows in set Fψ
        if (psi_f > tau_psi)
        {
            // df → achieved anonymity degree of flow f
            // (calculated earlier using Eq. 3.51)
            double d_f = anonymity_degree[fid];

            // Add anonymity value to total (Σ df)
            anonymity_sum += d_f;

            // Count this flow as a privacy-mode flow
            privacy_flow_counter++;
        }
    }

    // Compute average anonymity for the CURRENT routing cycle
    // d̄ = (1 / |Fψ|) * Σ df
    if (privacy_flow_counter > 0)
        current_anonymity_degree = anonymity_sum / privacy_flow_counter;
    else
        // If no privacy flows exist, anonymity is 0
        current_anonymity_degree = 0.0;

    // Accumulate anonymity values across cycles
    // This is used to compute overall average over time
    previous_cumulative_anonymity_degree += current_anonymity_degree;

    // Final average across all routing cycles so far
    // d̄_total = (1 / K) * Σ d̄_k
    average_anonymity_degree =
        previous_cumulative_anonymity_degree / data_gathering_cycle_number;

    // Print result for monitoring/debugging
    cout << "average anonymity degree is "
         << average_anonymity_degree << endl;
}